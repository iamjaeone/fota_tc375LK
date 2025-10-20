import socket
import tkinter as tk
from tkinter import scrolledtext, font as tkFont
import threading
import time

# 라즈베리파이의 실제 IP 주소를 입력합니다.
RPI_HOST = '172.26.14.254'
DOIP_PORT = 13400

# --- 세션 상태 관리를 위한 전역 변수 ---
g_extended_session_active = False
g_last_comm_time = 0.0
g_current_session_code = 0  # ✨ 현재 세션 코드를 추적하기 위한 변수

# --- 백그라운드 스레드 함수들 ---
def tester_present_thread():
    """백그라운드에서 주기적으로 Tester Present(SID 0x3E)를 보내는 스레드"""
    global g_last_comm_time
    while True:
        if g_extended_session_active and (time.time() - g_last_comm_time) > 2.0:
            try:
                print("[Keep-Alive] Tester Present 전송...")
                uds_request = bytes([0x3E, 0x80])
                doip_packet = wrap_in_doip(uds_request)
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.settimeout(2)
                    s.connect((RPI_HOST, DOIP_PORT))
                    s.sendall(doip_packet)
                g_last_comm_time = time.time()
            except Exception as e:
                print(f"[Keep-Alive] 에러: {e}")
        time.sleep(1)

def poll_session_status_thread():
    """백그라운드에서 주기적으로 현재 세션 상태(DID 0xF186)를 요청하고 GUI를 업데이트하는 스레드"""
    global g_current_session_code
    session_map = {1: "Default", 3: "Extended", 2: "Programming"}
    while True:
        try:
            uds_request = bytes([0x22, 0xF1, 0x86])
            # 이 통신은 Keep-Alive 타이머에 영향을 주지 않도록 update_time=False 사용
            uds_response = send_and_receive_doip(uds_request, update_time=False)
            
            if uds_response and uds_response[0] == 0x62:
                session_code = uds_response[3]
                session_name = session_map.get(session_code, f"Unknown (0x{session_code:02X})")
                session_status_label.config(text=f"현재 세션: {session_name}")

                # --- ✨ 추가된 부분: 세션 변경 감지 및 결과창 업데이트 ---
                # 이전 세션과 현재 세션이 다른지 확인
                if g_current_session_code != session_code:
                    # Extended(3)에서 Default(1)로 변경되었다면, 타임아웃으로 간주
                    if g_current_session_code == 3 and session_code == 1:
                        update_result_text("[+] Default 세션으로 복귀 (타임아웃)")
                    
                    # 현재 세션 상태를 전역 변수에 업데이트
                    g_current_session_code = session_code
                # --- ✨ 추가 끝 ---

            else:
                session_status_label.config(text="현재 세션: 확인 불가")
        except Exception:
            session_status_label.config(text="현재 세션: 연결 끊김")
        
        time.sleep(3)

# --- DID 설명 딕셔너리 ---
DID_DESCRIPTIONS = {
    0x1000: "레이저 센서 거리",
    0x2000: "초음파(좌) 센서 거리",
    0x2001: "초음파(우) 센서 거리",
    0x2002: "초음파(후) 센서 거리",
    0xF186: "현재 세션 정보",
    0xF187: "ECU 부품 번호",
    0xF18C: "ECU 시리얼 번호",
    0xF190: "차대번호 (VIN)",
    0xF192: "ECU 공급업체 정보",
    0xF193: "ECU 제조 날짜",
    0xF1A0: "지원 DID 목록"
}


# --- DoIP 헬퍼 함수들 ---
def wrap_in_doip(uds_payload):
    protocol_version = 0x02
    payload_type = 0x8001
    payload_length = len(uds_payload)
    doip_header = bytearray([protocol_version, protocol_version ^ 0xFF])
    doip_header.extend(payload_type.to_bytes(2, 'big'))
    doip_header.extend(payload_length.to_bytes(4, 'big'))
    return bytes(doip_header) + uds_payload

def unwrap_doip(doip_packet):
    if len(doip_packet) < 8 or doip_packet[0] != 0x02:
        raise ValueError("Invalid DoIP packet")
    payload_length = int.from_bytes(doip_packet[4:8], 'big')
    uds_message = doip_packet[8:]
    if len(uds_message) != payload_length:
        raise ValueError("DoIP payload length mismatch")
    return uds_message

def format_dtc(dtc_bytes):
    if len(dtc_bytes) != 3: return "Invalid DTC format"
    first_byte = dtc_bytes[0]
    dtc_type_map = {0b00: 'P', 0b01: 'C', 0b10: 'B', 0b11: 'U'}
    first_char = dtc_type_map.get(first_byte >> 6, '?')
    second_char = (first_byte >> 4) & 0x03
    last_three_chars = f"{(first_byte & 0x0F):X}{dtc_bytes[1]:02X}{dtc_bytes[2]:02X}"
    return f"{first_char}{second_char}{last_three_chars}"

# --- 공통 기능 함수 ---
def send_and_receive_doip(uds_payload, update_time=True, timeout=5):
    """DoIP 통신을 수행하고, 타임아웃을 설정할 수 있는 공통 함수"""
    global g_last_comm_time
    if update_time:
        g_last_comm_time = time.time()
    
    doip_request_packet = wrap_in_doip(uds_payload)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(timeout) # 파라미터로 받은 타임아웃 값을 사용합니다.
        s.connect((RPI_HOST, DOIP_PORT))
        s.sendall(doip_request_packet)
        doip_response_packet = s.recv(1024)
        return unwrap_doip(doip_response_packet)

def update_result_text(text):
    result_text.config(state=tk.NORMAL)
    result_text.delete(1.0, tk.END)
    result_text.insert(tk.END, text)
    result_text.config(state=tk.DISABLED)

# --- 각 버튼에 연결될 기능 함수들 ---
def control_session(session_type: int):
    global g_extended_session_active, g_current_session_code
    uds_request = bytes([0x10, session_type])
    session_map = {1: "Default", 2: "Programming", 3: "Extended"}
    session_name = session_map.get(session_type, "Unknown")
    update_result_text(f"[*] {session_name} 세션 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x50:
            update_result_text(f"[+] {session_name} 세션으로 성공적으로 전환되었습니다.")
            g_extended_session_active = (session_type == 0x03)
            g_current_session_code = session_type
        else:
            g_extended_session_active = False
            update_result_text(f"[-] 세션 전환 실패: {uds_response.hex().upper()}")
    except Exception as e:
        g_extended_session_active = False
        update_result_text(f"[!] 에러: {e}")

def request_generic_string(did, name):
    """문자열을 반환하는 DID를 요청하는 공통 함수"""
    uds_request = bytes([0x22]) + did.to_bytes(2, 'big')
    update_result_text(f"[*] DID 0x{did:04X} ({name}) 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x62:
            value = uds_response[3:].decode('utf-8', errors='ignore')
            update_result_text(f"[+] {name}: {value}")
        else:
            update_result_text(f"[-] ECU 부정 응답: {uds_response.hex().upper()}")
    except Exception as e:
        update_result_text(f"[!] 에러: {e}")

def request_laser_sensor_data():
    uds_request = bytes([0x22, 0x10, 0x00])
    update_result_text(f"[*] DID 0x1000 (레이저 센서) 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x62:
            sensor_value = int.from_bytes(uds_response[3:], 'big')
            update_result_text(f"[+] 레이저 센서 값: {sensor_value} mm")
        else:
            update_result_text(f"[-] ECU 부정 응답: {uds_response.hex().upper()}")
    except Exception as e:
        update_result_text(f"[!] 에러: {e}")

def request_ultrasonic_data(did, name):
    uds_request = bytes([0x22]) + did.to_bytes(2, 'big')
    update_result_text(f"[*] DID 0x{did:04X} ({name}) 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x62:
            scaled_value = int.from_bytes(uds_response[3:], 'big')
            distance_cm = scaled_value / 10.0
            update_result_text(f"[+] {name}: {distance_cm:.1f} cm")
        elif uds_response and uds_response[0] == 0x7F:
            update_result_text(f"[-] ECU 부정 응답: {uds_response.hex().upper()} (센서 측정 실패)")
        else:
            update_result_text(f"[-] ECU 응답 형식 오류: {uds_response.hex().upper()}")
    except Exception as e:
        update_result_text(f"[!] 에러: {e}")

def request_all_ultrasonic_data():
    """모든 초음파 센서 데이터를 순차적으로 요청하고 결과를 통합하여 표시"""
    update_result_text("[*] 모든 초음파 센서 데이터 요청 중...")
    sensors_to_query = [(0x2000, "좌측"), (0x2001, "우측"), (0x2002, "후방")]
    results = []
    has_error = False
    for did, name in sensors_to_query:
        try:
            uds_request = bytes([0x22]) + did.to_bytes(2, 'big')
            uds_response = send_and_receive_doip(uds_request)
            if uds_response and uds_response[0] == 0x62:
                scaled_value = int.from_bytes(uds_response[3:], 'big')
                distance_cm = scaled_value / 10.0
                results.append(f"  - {name}: {distance_cm:.1f} cm")
            else:
                results.append(f"  - {name}: 응답 오류 ({uds_response.hex().upper()})")
                has_error = True
        except Exception as e:
            results.append(f"  - {name}: 통신 에러 ({e})")
            has_error = True
            break
    final_text = ("[+] 모든 초음파 센서 값:\n" if not has_error else "[!] 일부 센서에서 오류가 발생했습니다:\n") + "\n".join(results)
    update_result_text(final_text)

def request_supported_dids():
    uds_request = bytes([0x22, 0xF1, 0xA0])
    update_result_text(f"[*] DID 0xF1A0 (지원 DID 목록) 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x62:
            did_data = uds_response[3:]
            did_list = []
            for i in range(0, len(did_data), 2):
                did_chunk = did_data[i:i+2]
                if len(did_chunk) == 2:
                    did_value = int.from_bytes(did_chunk, 'big')
                    description = DID_DESCRIPTIONS.get(did_value, "알 수 없는 DID")
                    did_list.append(f"  - 0x{did_value:04X}: {description}")
            update_result_text("[+] ECU가 지원하는 DID 목록:\n" + "\n".join(did_list))
        else:
            update_result_text(f"[-] ECU 부정 응답: {uds_response.hex().upper()}")
    except Exception as e:
        update_result_text(f"[!] 에러: {e}")

def request_dtc_data():
    uds_request = bytes([0x19, 0x02, 0xFF])
    update_result_text(f"[*] DTC 정보 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x59:
            dtc_data = uds_response[3:]
            if not dtc_data:
                update_result_text("[+] 고장 코드가 없습니다.")
            else:
                dtc_list = []
                for i in range(0, len(dtc_data), 4):
                    dtc_chunk = dtc_data[i:i+4]
                    if len(dtc_chunk) == 4:
                        dtc_code_bytes, dtc_status = dtc_chunk[0:3], dtc_chunk[3]
                        formatted_dtc = format_dtc(dtc_code_bytes)
                        dtc_list.append(f"  - {formatted_dtc} (상태: 0x{dtc_status:02X})")
                update_result_text("[+] 감지된 고장 코드:\n" + "\n".join(dtc_list))
        else:
            update_result_text(f"[-] ECU 부정 응답: {uds_response.hex().upper()}")
    except Exception as e:
        update_result_text(f"[!] 에러: {e}")

def write_aeb_flag(is_on: bool):
    new_data = 0x01 if is_on else 0x00
    uds_request = bytes([0x2E, 0x20, 0x00, new_data])
    status_text = 'ON' if is_on else 'OFF'
    update_result_text(f"[*] AEB 기능 {status_text} 요청 전송 중...")
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x6E:
            update_result_text(f"[+] AEB 기능이 성공적으로 {status_text} 되었습니다.")
        else:
            update_result_text(f"[-] ECU 부정 응답: {uds_response.hex().upper()}")
    except Exception as e:
        update_result_text(f"[!] 에러: {e}")

def start_motor_routine(rid, name):
    """모터 강제 구동 루틴(SID 0x31)을 요청하는 함수"""
    # UDS 요청 : SID 0x31, Sub-func 0x01(start), RID 2바이트
    uds_request = bytes([0x31, 0x01]) + rid.to_bytes(2, 'big')

    update_result_text(f"[*] {name} (RID 0x{rid:04X}) 요청 전송 중...")

    try:
        # 루틴 실행이 5초 걸리므로, 응답 타임아웃 넉넉하게 10초로 설정
        uds_response = send_and_receive_doip(uds_request, timeout=10)
        # 0x31에 대한 긍정 응답은 0x71
        if uds_response and uds_response[0] == 0x71:
            update_result_text(f"[+] {name}이 시작되었습니다.\n(ECU가 5초 간 모터 구동 후 정지합니다)")
        # 부정 응답인 경우
        else:
            update_result_text(f"[-] 루틴 시작 실패 : {uds_response.hex().upper()}")
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")

# --- GUI 생성 ---
window = tk.Tk()
window.title("ECU 진단 툴")
window.geometry("600x800")
window.configure(bg="#f0f0f0")

default_font = tkFont.nametofont("TkDefaultFont")
default_font.configure(family="맑은 고딕", size=9)

session_status_label = tk.Label(window, text="현재 세션: 확인 중...", font=("맑은 고딕", 9, "italic"), bg="#f0f0f0", fg="blue")
session_status_label.pack(pady=(5, 0))

# --- 1. 센서 데이터 그룹 ---
sensor_frame = tk.LabelFrame(window, text=" 센서 데이터 ", padx=10, pady=5, bg="#f0f0f0")
sensor_frame.pack(pady=5, padx=10, fill="x")
laser_button = tk.Button(sensor_frame, text="레이저", command=request_laser_sensor_data)
laser_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
left_us_button = tk.Button(sensor_frame, text="초음파(좌)", command=lambda: request_ultrasonic_data(0x2000, "좌측 초음파"))
left_us_button.grid(row=0, column=1, padx=5, pady=5, sticky="ew")
right_us_button = tk.Button(sensor_frame, text="초음파(우)", command=lambda: request_ultrasonic_data(0x2001, "우측 초음파"))
right_us_button.grid(row=0, column=2, padx=5, pady=5, sticky="ew")
rear_us_button = tk.Button(sensor_frame, text="초음파(후방)", command=lambda: request_ultrasonic_data(0x2002, "후방 초음파"))
rear_us_button.grid(row=0, column=3, padx=5, pady=5, sticky="ew")
all_us_button = tk.Button(sensor_frame, text="초음파(모두)", command=request_all_ultrasonic_data, font=("맑은 고딕", 9, "bold"))
all_us_button.grid(row=0, column=4, padx=5, pady=5, sticky="ew")
for i in range(5): sensor_frame.grid_columnconfigure(i, weight=1)

# --- 2. ECU 정보 그룹 ---
info_frame = tk.LabelFrame(window, text=" ECU 정보 ", padx=10, pady=5, bg="#f0f0f0")
info_frame.pack(pady=5, padx=10, fill="x")
part_number_button = tk.Button(info_frame, text="부품 번호", command=lambda: request_generic_string(0xF187, "ECU 부품 번호"))
part_number_button.grid(row=0, column=0, padx=5, pady=5, sticky="ew")
serial_number_button = tk.Button(info_frame, text="시리얼 번호", command=lambda: request_generic_string(0xF18C, "ECU 시리얼 번호"))
serial_number_button.grid(row=0, column=1, padx=5, pady=5, sticky="ew")
vin_button = tk.Button(info_frame, text="차대번호(VIN)", command=lambda: request_generic_string(0xF190, "차대번호(VIN)"))
vin_button.grid(row=0, column=2, padx=5, pady=5, sticky="ew")
mfg_date_button = tk.Button(info_frame, text="제조 날짜", command=lambda: request_generic_string(0xF193, "ECU 제조 날짜"))
mfg_date_button.grid(row=1, column=0, padx=5, pady=5, sticky="ew")
supplier_button = tk.Button(info_frame, text="공급업체", command=lambda: request_generic_string(0xF192, "ECU 공급업체"))
supplier_button.grid(row=1, column=1, padx=5, pady=5, sticky="ew")
supported_dids_button = tk.Button(info_frame, text="지원 DID 목록", command=request_supported_dids)
supported_dids_button.grid(row=1, column=2, padx=5, pady=5, sticky="ew")
for i in range(3): info_frame.grid_columnconfigure(i, weight=1)

# --- 3. 진단 및 제어 그룹 ---
control_frame = tk.LabelFrame(window, text=" 진단 및 제어 ", padx=10, pady=5, bg="#f0f0f0")
control_frame.pack(pady=5, padx=10, fill="x")

dtc_button = tk.Button(control_frame, text="DTC 정보 읽기", command=request_dtc_data)
dtc_button.grid(row=0, column=0, columnspan=2, padx=5, pady=5, sticky="ew")

aeb_on_button = tk.Button(control_frame, text="AEB 기능 ON", command=lambda: write_aeb_flag(True))
aeb_on_button.grid(row=0, column=2, padx=5, pady=5, sticky="ew")

aeb_off_button = tk.Button(control_frame, text="AEB 기능 OFF", command=lambda: write_aeb_flag(False))
aeb_off_button.grid(row=0, column=3, padx=5, pady=5, sticky="ew")

motor_forward_button = tk.Button(control_frame, text="모터 정회전 테스트", command=lambda: start_motor_routine(0x0001, "모터 정회전 테스트"))
motor_forward_button.grid(row=2, column=0, columnspan=2, padx=5, pady=5, sticky="ew")

motor_reverse_button = tk.Button(control_frame, text="모터 역회전 테스트", command=lambda: start_motor_routine(0x0002, "모터 역회전 테스트"))
motor_reverse_button.grid(row=2, column=2, columnspan=2, padx=5, pady=5, sticky="ew")

session_extended_button = tk.Button(control_frame, text="세션 시작 (Ext)", command=lambda: control_session(0x03))
session_extended_button.grid(row=1, column=0, columnspan=2, padx=5, pady=5, sticky="ew")

session_default_button = tk.Button(control_frame, text="세션 종료 (Def)", command=lambda: control_session(0x01))
session_default_button.grid(row=1, column=2, columnspan=2, padx=5, pady=5, sticky="ew")

for i in range(4): control_frame.grid_columnconfigure(i, weight=1)



# --- 결과 텍스트 창 ---
result_text = scrolledtext.ScrolledText(window, height=8, font=("Consolas", 14))
result_text.pack(pady=10, padx=10, fill="both", expand=True)
result_text.insert(tk.END, "버튼을 눌러 진단을 시작하세요.")
result_text.config(state=tk.DISABLED)

if __name__ == "__main__":
    # 백그라운드 스레드들을 생성하고 시작
    #tp_thread = threading.Thread(target=tester_present_thread, daemon=True)
    #tp_thread.start()
    
    session_poll_thread = threading.Thread(target=poll_session_status_thread, daemon=True)
    session_poll_thread.start()


    window.mainloop()