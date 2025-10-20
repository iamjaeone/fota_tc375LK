from udsoncan.client import Client
from udsoncan.services import RoutineControl
from udsoncan.common.MemoryLocation import MemoryLocation


# ==================== Configuration =======================
CAN_INTERFACE = 'can0'
CAN_BITRATE = 500000
REQUEST_ID = 0x7E0
RESPONSE_ID = 0x7E8
# ==========================================================

global g_max_number_of_bytes
g_max_number_of_bytes = 0

def diagnostic_session_control(client: Client, session_type: int) -> bool:
    print(f"Switching to diagnostic session type 0x{session_type:02X}...")
    response = client.change_session(newsession=session_type)
    print(f"✅ Switched to session type 0x{session_type:02X} successfully.")
    return True

def request_download(client: Client, start_addr: int, size: int) -> bool:
        mem_loc = MemoryLocation(address=start_addr, memorysize=size)
        print(f"Requesting download at address 0x{start_addr:08X} of size {size} bytes...")
        response = client.request_download(memory_location=mem_loc)
        global g_max_number_of_bytes
        if(response.data[0] == 1):
            g_max_number_of_bytes = response.data[1]
        elif(response.data[0] == 2):
            g_max_number_of_bytes = int.from_bytes(response.data[1:3], 'big')
        else:
            raise ValueError("Unsupported length of maxNumberOfBlockLength from ECU")
        print(f"✅ Download request accepted. Max block size: {g_max_number_of_bytes}")
        return True

def routine_control_erase_flash(client: Client, start_addr: int, size: int) -> bool:
        mem_loc = MemoryLocation(address=start_addr, memorysize=size)
        print(f"Requesting flash erase at address 0x{start_addr:08X} of size {size} bytes...")
        erase_params = start_addr.to_bytes(4, 'big') + size.to_bytes(4, 'big')
        response = client.routine_control(routine_id=0xFF00, control_type=RoutineControl.ControlType.startRoutine, data=erase_params)
        print("✅ Flash erase started successfully.")
        return True

def transfer_data_blob(client: Client, data_blob: bytearray) -> bool:
    global g_max_number_of_bytes
    if g_max_number_of_bytes == 0:
        raise ValueError("Max number of bytes per block is not set. Please perform a request download first.")
    
    print(f"Transferring data blob of size {len(data_blob)} bytes...")
    block_sequence_counter = 1
    for i in range(0, len(data_blob), g_max_number_of_bytes - 2):
        payload_size = g_max_number_of_bytes - 2
        chunk = data_blob[i : i + payload_size]
        response = client.transfer_data(sequence_number=block_sequence_counter, data=bytes(chunk))
        block_sequence_counter = (block_sequence_counter + 1) % 0x100
    print("✅ Data transfer completed successfully.")
    return True

def request_transfer_exit(client: Client) -> bool:
    print("Requesting transfer exit...")
    response = client.request_transfer_exit()
    global g_max_number_of_bytes
    g_max_number_of_bytes = 0
    print("✅ Transfer exit completed successfully.")
    return True

def ecu_reset(client: Client, reset_type: int) -> bool:
    print(f"Sending ECU Reset command with type 0x{reset_type:02X}...")
    response = client.ecu_reset(reset_type=reset_type)
    print("✅ ECU Reset command sent successfully.")
    return True