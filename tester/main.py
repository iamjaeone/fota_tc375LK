import can
import isotp
from udsoncan.connections import PythonIsoTpConnection
import udsoncan

from udsoncan import configs

from uds import *
from hex_parser import get_block

configs.default_client_config['request_timeout'] = 10
configs.default_client_config['p2_timeout'] = 10


def run_flashing_process():
    print("=========================================")
    print("      UDS Firmware Update")
    print("=========================================")

    structured_data = get_block()

    if not structured_data:
        print("❌ Error: Failed to process firmware data. Stopping the process.")
        return
    print(f"✅ Parsing complete: Found {len(structured_data)} Chunks.")

    bus = can.interface.Bus(channel=CAN_INTERFACE, interface='socketcan', bitrate=CAN_BITRATE)
    tp_addr = isotp.Address(isotp.AddressingMode.Normal_11bits, txid=REQUEST_ID, rxid=RESPONSE_ID)
    stack = isotp.CanStack(bus=bus, address=tp_addr)
    conn = PythonIsoTpConnection(stack)

    try:
        with Client(conn, request_timeout=10) as client:
            diagnostic_session_control(client, 0x02)  # Switch to programming session
            for i, sector in enumerate(structured_data):
                routine_control_erase_flash(client, sector['sector_start_addr'], sector['num_sectors'] * 0x4000)
                for i, block in enumerate(sector['block']):
                    request_download(client, block['address'], len(block['data']))
                    transfer_data_blob(client, block['data'])
                    request_transfer_exit(client)
                    print(f"✅ Block {i+1}/{len(sector['block'])} in Sector {i+1}/{len(structured_data)} flashed successfully.")
                print(f"✅ Sector {i+1}/{len(structured_data)} flashed successfully.")
            ecu_reset(client, 0x01)  # Hard Reset              
        print("🎉 Firmware update process completed successfully!")
    except udsoncan.exceptions.NegativeResponseException as e:
        print(f"❌ Negative response from ECU: Service={e.response.service.name}, Code={e.response.code_name} ({hex(e.response.code)})")
        return False
    except Exception as e:
        print(f"\n🚨 Error: {e}")
    finally:
        bus.shutdown()


if __name__ == "__main__":
    run_flashing_process()