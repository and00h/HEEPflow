#!/usr/bin/env python3

from string import Template
import argparse
import os.path
import sys
import binascii


parser = argparse.ArgumentParser(description='Convert binary file to verilog rom')
parser.add_argument('filename', metavar='filename', nargs=1,
                   help='filename of input binary')

args = parser.parse_args()
file = args.filename[0];

# check that file exists
if not os.path.isfile(file):
    print("File {} does not exist.".format(filename))
    sys.exit(1)

filename, ext = os.path.splitext(file)

module = """\
module tflite_rom
  import reg_pkg::*;
(
  input  reg_req_t     reg_req_i,
  output reg_rsp_t     reg_rsp_o
);
  import core_v_mini_mcu_pkg::*;
  localparam int unsigned ModelSize = $size;

  logic [ModelSize-1:0][31:0] mem;
  assign mem = {
$content
  };

  logic [$$clog2(core_v_mini_mcu_pkg::TFLITE_ROM_SIZE)-1-2:0] word_addr;
  logic [$$clog2(ModelSize)-1:0] rom_addr;

  assign word_addr = reg_req_i.addr[$$clog2(core_v_mini_mcu_pkg::TFLITE_ROM_SIZE)-1:2];
  assign rom_addr  = word_addr[$$clog2(ModelSize)-1:0];

  assign reg_rsp_o.error = 1'b0;
  assign reg_rsp_o.ready = 1'b1;

  always_comb begin
    if (word_addr > (ModelSize-1)) begin
      reg_rsp_o.rdata = '0;
    end else begin
      reg_rsp_o.rdata = mem[rom_addr];
    end
  end

endmodule
"""

c_var = """\
// Auto-generated code

const unsigned int model_data_size = $size;

alignas(16) const unsigned char model_data[model_data_size] = {
$content
};
"""

def read_bin():

    with open(file, 'rb') as f:
        rom = f.read().hex(' ').split(' ')
    
    align = (int((len(rom) + 3) / 4 )) * 4;

    for i in range(len(rom), align):
        rom.append("00")
        
    return rom

rom = read_bin()

""" Generate C header file for simulator
"""
with open("tflite_rom.h", "w") as f:
    rom_str = ""
    for i in range(int(len(rom) / 4)):
      rom_str += '0x' + ''.join(rom[i*4:i*4+4][::1]) + ',\n'
    rom_str = rom_str[:-2]
    s = Template(c_var)
    f.write(s.substitute(filename=filename, size=int(len(rom)/4), content=rom_str))
    f.close()

""" Generate SystemVerilog ROM for FPGA and ASIC
"""
with open("tflite_rom.sv", "w") as f:
    rom_str = ""
    for i in reversed(range(int(len(rom) / 4))):
      rom_str += "32'h" + ''.join(rom[i*4:i*4+4][::-1]) + ',\n'
    rom_str = rom_str[:-2]

    s = Template(module)
    f.write(s.substitute(filename=filename, size=int(len(rom)/4), content=rom_str))
