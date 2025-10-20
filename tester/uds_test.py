import can
import isotp
from udsoncan.connections import PythonIsoTpConnection
import udsoncan

from uds import *


# ==================== 사용자 설정 영역 ====================
# CAN 버스 설정
CAN_INTERFACE = 'can0'
CAN_BITRATE = 500000
REQUEST_ID = 0x7E0
RESPONSE_ID = 0x7E8
# ==========================================================

def print_menu():
    """메뉴를 터미널에 출력하는 함수"""
    print("\n\n===== UDS ECU 기능 테스터 =====")
    print("1. Diagnostic Session Control (0x10)")
    print("2. Request Download (0x34)")
    print("3. Routine Control - Flash Erase (0x31)")
    print("4. Transfer Data (0x36)")
    print("5. Request Transfer Exit (0x37)")
    print("6. ECU Reset (0x11)")
    print("--------------------------------")
    print("0. 종료")
    return input("실행할 기능 번호를 입력하세요: ")

def test_diagnostic_session_control(client: Client):
    """Diagnostic Session Control (0x10) 서비스 테스트"""
    try:
        diagnostic_session_control(client, 0x02)
        print(f"✅ 성공: 세션 타입이 Programming Session으로 변경되었습니다.")
    except Exception as e:
        print(f"❌ 오류 발생: {e}")

def test_request_download(client: Client):
    """Request Download (0x34) 서비스 테스트"""
    try:
        address_str = input("다운로드할 메모리 주소를 입력하세요 (예: 0x08020000): ")
        size_str = input("다운로드할 데이터 크기(bytes)를 입력하세요 (예: 123456): ")
        
        address = int(address_str, 16)
        size = int(size_str)

        print(f"Request Download (주소: {hex(address)}, 크기: {size}) 요청을 보냅니다...")

        request_download(client, address, size)
    except ValueError:
        print("❌ 오류: 올바른 숫자 형식이 아닙니다.")
    except Exception as e:
        print(f"❌ 오류 발생: {e}")


def test_routine_control_erase_flash(client: Client):
    """Routine Control (0x31) 서비스로 Flash Erase 테스트"""
    try:
        address_str = input("지울 메모리 주소를 입력하세요 (예: 0x08020000): ")
        size_str = input("지울 메모리 크기(bytes)를 입력하세요 (예: 131072): ")
        
        address = int(address_str, 16)
        size = int(size_str)
        
        print(f"Routine Control - Flash Erase (주소: {hex(address)}, 크기: {size}) 요청을 보냅니다...")
        routine_control_erase_flash(client, address, size)
    except ValueError:
        print("❌ 오류: 올바른 숫자 형식이 아닙니다.")
    except Exception as e:
        print(f"❌ 오류 발생: {e}")


def test_transfer_data(client: Client):
    try:
        data = input("전송할 데이터를 16진수 문자열로 입력하세요 (예: DEADBEEF): ")

        data = bytes.fromhex(data)
        print(f"Transfer Data (데이터 길이: {len(data)}) 요청을 보냅니다...")

        transfer_data_blob(client, data)
    except ValueError:
        print("❌ 오류: 올바른 숫자 형식이 아닙니다.")
    except Exception as e:
        print(f"❌ 오류 발생: {e}")


def test_request_transfer_exit(client: Client):
    try:
        print("Request Transfer Exit 요청을 보냅니다...")
        request_transfer_exit(client)
    except Exception as e:
        print(f"❌ 오류 발생: {e}")


def test_ecu_reset(client: Client):
    """ECU Reset (0x11) 서비스 테스트"""
    try:
        print(f"ECU에 Reset(Type: 0x01(Hard Reset)) 요청을 보냅니다...")
        ecu_reset(client, 0x01)
        print("✅ 성공: ECU 리셋 요청을 보냈습니다 (ECU가 재부팅될 수 있습니다).")
    except Exception as e:
        print(f"❌ 오류 발생: {e}")


def main():
    bus = can.interface.Bus(channel=CAN_INTERFACE, interface='socketcan', bitrate=CAN_BITRATE)
    tp_addr = isotp.Address(isotp.AddressingMode.Normal_11bits, txid=REQUEST_ID, rxid=RESPONSE_ID)
    stack = isotp.CanStack(bus=bus, address=tp_addr)
    conn = PythonIsoTpConnection(stack)

    try:
        with Client(conn, request_timeout=5) as client:
            while True:
                choice = print_menu()
                if choice == '1':
                    test_diagnostic_session_control(client)
                elif choice == '2':
                    test_request_download(client)
                elif choice == '3':
                    test_routine_control_erase_flash(client)
                elif choice == '4':
                    test_transfer_data(client)
                elif choice == '5':
                    test_request_transfer_exit(client)
                elif choice == '6':
                    test_ecu_reset(client)
                elif choice == '0':
                    print("테스터를 종료합니다.")
                    break
                else:
                    print("잘못된 입력입니다. 메뉴에 있는 번호를 입력해주세요.")
    except KeyboardInterrupt:
        print("\n테스터를 종료합니다.")
    except udsoncan.exceptions.NegativeResponseException as e:
        print(f"❌ ECU 부정 응답: 서비스={e.response.service.name}, 코드={e.response.code_name} ({hex(e.response.code)})")
    except Exception as e:
        print(f"❌ 알 수 없는 오류 발생: {e}")
    finally:
        bus.shutdown()
if __name__ == '__main__':
    main()