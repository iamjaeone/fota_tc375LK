from pc_doip_client import send_and_receive_doip, update_result_text
import socket


def diagnostic_session_control(session_type: int):
    """
    UDS Service 0x10: Diagnostic Session Control
    """
    uds_request = bytes([0x10, session_type])
    
    update_result_text(f"[*] 진단 세션 제어 요청: 세션 타입=0x{session_type:02X}")
    
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x50:
            update_result_text(f"[+] 진단 세션 제어 성공. 현재 세션 타입=0x{session_type:02X}")
        else:
            update_result_text(f"[-] 진단 세션 제어 실패: {uds_response.hex().upper()}")
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")

def routine_control(sub_function: int, routine_identifier: int, address: int, size: int):
    """
    UDS Service 0x31: Routine Control (startRoutine, RID 0xFF00)
    """
    erase_params = address.to_bytes(4, 'big') + size.to_bytes(4, 'big')
    uds_request = bytes([0x31]) + sub_function.to_bytes(1, 'big') + routine_identifier.to_bytes(2, 'big') + erase_params
    
    try:
        update_result_text(f"[*] 플래시 Erase 요청: 주소=0x{address:X}, 크기={size}B")
        uds_response = send_and_receive_doip(uds_request, timeout=5) 
        if uds_response and uds_response[0] == 0x71:
            update_result_text(f"[+] 플래시 Erase 성공.")
        else:
            update_result_text(f"[-] 플래시 Erase 요청 실패: {uds_response.hex().upper()}")
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")

def request_download(address: int, size: int) -> int:
    """
    UDS Service 0x34: Request Download
    """
    uds_request = bytes([0x34, 0x00, 0x44]) + address.to_bytes(4, 'big') + size.to_bytes(4, 'big')
    
    update_result_text(f"[*] 다운로드 요청: 주소=0x{address:X}, 크기={size}B")
    
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x74:
            len_format = uds_response[1]
            max_block_size = int.from_bytes(uds_response[2:2+len_format], 'big')
            update_result_text(f"[+] 다운로드 요청 성공. 최대 블록 크기: {max_block_size} 바이트")
            return max_block_size
        else:
            error_msg = f"[-] 다운로드 요청 실패: {uds_response.hex().upper()}"
            update_result_text(error_msg)
            return 0
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
        return 0
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")
        return 0

def transfer_data(block_sequence_counter: int, data: bytes):
    """
    UDS Service 0x36: Transfer Data
    """
    uds_request = bytes([0x36, block_sequence_counter]) + data


    update_result_text(f"[*] 데이터 전송 (seq={block_sequence_counter}, size={len(data)}B)...")

    try:
        uds_response = send_and_receive_doip(uds_request, timeout=10)
        if uds_response and uds_response[0] == 0x76: # 0x36에 대한 긍정 응답
            update_result_text(f"[+] 데이터 전송 성공 (seq={block_sequence_counter}).")
        else:
            update_result_text(f"[-] 데이터 전송 실패 (seq={block_sequence_counter}): {uds_response.hex().upper()}")
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")        
    
def request_transfer_exit():
    """
    UDS Service 0x37: Request Transfer Exit
    """
    uds_request = bytes([0x37])
    update_result_text(f"[*] 데이터 전송 종료 요청...")
    
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x77:
            update_result_text(f"[+] 데이터 전송 종료 성공.")
        else:
            update_result_text(f"[-] 전송 종료 요청 실패: {uds_response.hex().upper()}")
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")

def perform_ecu_reset(reset_type: int):
    """
    UDS Service 0x11: ECU Reset
    """
    uds_request = bytes([0x11, reset_type])

    update_result_text(f"[*] ECU 리셋 요청 (0x{reset_type:02X})...")
    
    try:
        uds_response = send_and_receive_doip(uds_request)
        if uds_response and uds_response[0] == 0x51: # 0x11에 대한 긍정 응답
            update_result_text(f"[+] ECU 리셋 성공.")
        else:
            update_result_text( f"[-] ECU 리셋 실패: {uds_response.hex().upper()}")
    except socket.timeout:
        update_result_text("[!] 에러: ECU로부터 응답 시간 초과.\n(ECU 루틴 실행이 너무 길거나, 응답이 없습니다)")
    except Exception as e:
        update_result_text(f"[!] 에러 : {e}")