from uds import ecu_reset
from udsondoip import *
from hex_parser import get_block
from pc_doip_client import control_session


global g_max_block_size
g_max_block_size = 0

def transfer_data_blob(data_blob: bytearray):
    global g_max_block_size
    if g_max_block_size == 0:
        print("❌ Error: Max block size is not set. Please perform a request download first.")
        return
    print(f"Transferring data blob of size {len(data_blob)} bytes...")
    block_sequence_counter = 1
    for i in range(0, len(data_blob), g_max_block_size - 2):
        payload_size = g_max_block_size - 2
        chunk = data_blob[i : i + payload_size]
        transfer_data(sequence_number=block_sequence_counter, data=bytes(chunk))
        block_sequence_counter = (block_sequence_counter + 1) % 0x100
    print("✅ Data transfer completed successfully.")



def run_ota_process(filepath: str):
    global g_max_block_size
    structured_data = get_block(filepath)
    if not structured_data:
        print("❌ Error: Failed to process firmware data. Stopping the process.")
        return
    
    control_session(0x02)  # Switch to programming session
    for i, sector in enumerate(structured_data):
        routine_control(0x01, 0xFF00, sector['sector_start_addr'], sector['num_sectors'] * 0x4000)
        for i, block in enumerate(sector['block']):
            g_max_block_size = request_download(block['address'], len(block['data']))
            transfer_data_blob(block['data'])
            request_transfer_exit()
            print(f"✅ Block {i+1}/{len(sector['block'])} in Sector {i+1}/{len(structured_data)} flashed successfully.")
        print(f"✅ Sector {i+1}/{len(structured_data)} flashed successfully.")
    ecu_reset(0x01)  # Hard Reset       


if __name__ == "__main__":
    filepath = input("Enter the path to the firmware file: ")
    run_ota_process(filepath)
