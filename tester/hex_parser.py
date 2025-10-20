def get_memory_properties(address: int) -> dict:
    # Pflash
    if 0xA0000000 <= address < 0xA0600000:
        return {'sector_size': 0x4000, 'page_size': 0x20}  # 16KB Sector, 32B Page
    
    # UCB: Skip
    if 0xAF400000 <= address < 0xAF406000:
        return {'sector_size': 0, 'page_size': 0}
    
    else:
        return None

def parse_hex_to_hardware_structure(filepath: str) -> dict:
    memory_map = {}
    extended_linear_address = 0

    try:
        with open(filepath, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line.startswith(':'): continue
                
                hex_data = bytes.fromhex(line[1:])
                byte_count, address_offset, record_type = hex_data[0], int.from_bytes(hex_data[1:3], 'big'), hex_data[3]
                data, checksum = hex_data[4:-1], hex_data[-1]
                
                if (sum(hex_data[:-1]) + checksum) & 0xFF != 0:
                    raise ValueError(f"Check Sum Error in line {line_num}")

                if record_type == 0x04:
                    extended_linear_address = int.from_bytes(data, 'big') << 16
                elif record_type == 0x00:
                    full_address = extended_linear_address + address_offset
                    base_addr = (full_address & 0x0FFFFFFF) | 0xA0000000 if (full_address >> 28) == 0x8 else full_address
                    
                    props = get_memory_properties(base_addr)
                    if props is None:
                        raise ValueError(f"Unsupported memory address 0x{base_addr:08X} in line {line_num}")
                    
                    if props['page_size'] == 0:
                        continue
                    
                    for i, byte_val in enumerate(data):
                        memory_map[base_addr + i] = byte_val
                elif record_type == 0x01:
                    break
    except FileNotFoundError:
        print(f"Error: File not found - {filepath}")
        return None
    except Exception as e:
        print(f"Error: An error occurred while parsing the file - {e}")
        return None

    if not memory_map: return {}
    
    structured_data = {}
    for addr in sorted(memory_map.keys()):
        props = get_memory_properties(addr)
        if not props or props['page_size'] == 0: continue

        sector_size, page_size = props['sector_size'], props['page_size']
        sector_addr = (addr // sector_size) * sector_size
        page_addr = (addr // page_size) * page_size
        
        if sector_addr not in structured_data:
            structured_data[sector_addr] = {'sector_address': sector_addr, 'pages': {}}
        if page_addr not in structured_data[sector_addr]['pages']:
            structured_data[sector_addr]['pages'][page_addr] = bytearray([0xFF] * page_size)
        
        offset_in_page = addr % page_size
        structured_data[sector_addr]['pages'][page_addr][offset_in_page] = memory_map[addr]
    
    final_structure = {}
    for sector_addr in sorted(structured_data.keys()):
        pages_dict = structured_data[sector_addr]['pages']
        if not pages_dict: continue

        props = get_memory_properties(sector_addr)
        page_size = props['page_size']
        
        sorted_page_addresses = sorted(pages_dict.keys())
        merged_chunks = []
        
        current_chunk_addr = sorted_page_addresses[0]
        current_chunk_data = bytearray(pages_dict[current_chunk_addr])

        for i in range(1, len(sorted_page_addresses)):
            prev_addr, current_addr = sorted_page_addresses[i-1], sorted_page_addresses[i]

            if current_addr == prev_addr + page_size:
                current_chunk_data.extend(pages_dict[current_addr])
            else:
                merged_chunks.append({'address': current_chunk_addr, 'data': current_chunk_data})
                current_chunk_addr = current_addr
                current_chunk_data = bytearray(pages_dict[current_addr])

        merged_chunks.append({'address': current_chunk_addr, 'data': current_chunk_data})

        final_structure[sector_addr] = {
            'sector_address': sector_addr,
            'data_chunks': merged_chunks
        }
            
    return final_structure
def merge_consecutive_sectors(sector_data: dict) -> list:
    if not sector_data:
        return []
    
    sorted_sector_addresses = sorted(sector_data.keys())
    programming_blocks = []
    
    current_block_start_addr = sorted_sector_addresses[0]
    current_block_chunks = sector_data[current_block_start_addr]['data_chunks']
    current_sector_count = 1 

    for i in range(1, len(sorted_sector_addresses)):
        prev_sector_addr = sorted_sector_addresses[i-1]
        current_sector_addr = sorted_sector_addresses[i]
        
        props = get_memory_properties(prev_sector_addr)
        if not props or props['sector_size'] == 0:
            continue
        prev_sector_size = props['sector_size']
        
        if current_sector_addr == prev_sector_addr + prev_sector_size:
            current_block_chunks.extend(sector_data[current_sector_addr]['data_chunks'])
            current_sector_count += 1
        else:
            programming_blocks.append({
                'sector_start_addr': current_block_start_addr,
                'num_sectors': current_sector_count,
                'block': current_block_chunks
            })
            current_block_start_addr = current_sector_addr
            current_block_chunks = sector_data[current_sector_addr]['data_chunks']
            current_sector_count = 1
            
    programming_blocks.append({
        'sector_start_addr': current_block_start_addr,
        'num_sectors': current_sector_count,
        'block': current_block_chunks
    })
    
    return programming_blocks


def get_block(filepath: str) -> list:
    try:
        hex_file_path = filepath.strip()
        if not hex_file_path.lower().endswith('.hex'):
            hex_file_path += ".hex"

        sector_by_sector_data = parse_hex_to_hardware_structure(hex_file_path)
        if sector_by_sector_data is not None:
            final_blocks = merge_consecutive_sectors(sector_by_sector_data)
            if not final_blocks:
                print("❌ Error: No valid memory sectors found in the HEX file.")
                return None
            else:
                return final_blocks
        else:
            print("❌ Error: Failed to parse the HEX file.")
            return None
    except Exception as e:
        print(f"\n🚨 Error: {e}")



if __name__ == "__main__":
    hex_file_path = input("Enter the path to the HEX file: ")
    data = get_block(hex_file_path)
    print(f"Parsed {len(data)} sectors.")
    for i, sector in enumerate(data):
        print(f"Sector {i+1}: Start Address = 0x{sector['sector_start_addr']:08X}, Number of Sectors = {sector['num_sectors']}, Number of Blocks = {len(sector['block'])}")
        for j, block in enumerate(sector['block']):
            print(f"  Block {j+1}: Address = 0x{block['address']:08X}, Size = {len(block['data'])} bytes")
            for k in range(0, len(block['data']), 4088):
                print(f"    Data Chunk {k//4088 + 1} First 16 bytes: {block['data'][k:k+16].hex().upper()} ...", end='\t')
                print(f"{block['data'][k+len(block['data'][k:k+4088])-16:k+len(block['data'][k:k+4088])].hex().upper()}")