#include "proto/jtag_cmd.pb.h"

#include <iostream>
#include <string>
#include <fstream>

using namespace usb2jtag;
using namespace std;

#define PERIOD 4

void build_lpc1850(PROTOCOLS* protocols);
void build_zedboard(PROTOCOLS* protocols);

int main(int argc, char** argv) {

  if( argc < 2 ) {
    printf("Usage: ./%s <output_file> ", argv[0]);
    return 1;
  }

  PROTOCOLS* protocols = new PROTOCOLS();

  build_lpc1850(protocols);

  build_zedboard(protocols);

  google::protobuf::ShutdownProtobufLibrary();
  {
    // Write the new address book back to disk.
    fstream output(argv[1], ios::out | ios::trunc | ios::binary);
    if (!protocols->SerializeToOstream(&output)) {
      cerr << "Failed to write protocol." << endl;
      return -1;
    }
  }

  google::protobuf::ShutdownProtobufLibrary();
}

void build_zedboard(PROTOCOLS* protocols) {

  PROTOCOL* protocol = protocols->add_protocols();

  protocol->set_target("zedboard");

  //auto hl_command_read  = protocol->add_commands();
  auto hl_command_write = protocol->add_commands();
  auto hl_command_reset = protocol->add_commands();
  auto hl_command_read  = protocol->add_commands();
 
  hl_command_read->set_type(HL_COMMAND_CMD_TYPE_READ);
  hl_command_read->set_size(64*5);

  hl_command_write->set_type(HL_COMMAND_CMD_TYPE_WRITE);
  hl_command_write->set_size(64*3);
  
  hl_command_reset->set_type(HL_COMMAND_CMD_TYPE_RESET);
  hl_command_reset->set_size(64*8);
  
  auto jtag_read_cmd1 = hl_command_read->add_commands();
  auto jtag_read_cmd2 = hl_command_read->add_commands();
  auto jtag_read_cmd3 = hl_command_read->add_commands();
  auto jtag_read_cmd4 = hl_command_read->add_commands();
  auto jtag_read_cmd5 = hl_command_read->add_commands();

  auto jtag_write_cmd1 = hl_command_write->add_commands();
  auto jtag_write_cmd2 = hl_command_write->add_commands();
  auto jtag_write_cmd3 = hl_command_write->add_commands();

  auto jtag_reset_cmd1 = hl_command_reset->add_commands();
  auto jtag_reset_cmd2 = hl_command_reset->add_commands();
  auto jtag_reset_cmd3 = hl_command_reset->add_commands();
  auto jtag_reset_cmd4 = hl_command_reset->add_commands();
  auto jtag_reset_cmd5 = hl_command_reset->add_commands();
  auto jtag_reset_cmd6 = hl_command_reset->add_commands();
  auto jtag_reset_cmd7 = hl_command_reset->add_commands();
  //auto jtag_reset_cmd8 = hl_command_reset->add_commands();
  
  // READ COMMAND
  
  jtag_read_cmd1->set_jtag_start(4);
  jtag_read_cmd1->set_jtag_end(0);
  jtag_read_cmd1->set_bitcount(36);
  jtag_read_cmd1->set_period(PERIOD);
  jtag_read_cmd1->set_payload(0x2);
  jtag_read_cmd1->set_data(0);
  jtag_read_cmd1->set_data_offset(3);
  jtag_read_cmd1->set_data_length(8);
  jtag_read_cmd1->set_data_is_address(true);

  jtag_read_cmd2->set_jtag_start(1);
  jtag_read_cmd2->set_jtag_end(0);
  jtag_read_cmd2->set_bitcount(32);
  jtag_read_cmd2->set_period(PERIOD);

  jtag_read_cmd3->set_jtag_start(4);
  jtag_read_cmd3->set_jtag_end(0);
  jtag_read_cmd3->set_bitcount(36);
  jtag_read_cmd3->set_period(PERIOD);
  jtag_read_cmd3->set_payload(0x100000007);
  jtag_read_cmd3->set_data(0);
  jtag_read_cmd3->set_data_offset(3);
  jtag_read_cmd3->set_data_length(9);
  jtag_read_cmd3->set_data_is_other(true);
  jtag_read_cmd3->set_data_is_address(false);

  jtag_read_cmd4->set_jtag_start(1);
  jtag_read_cmd4->set_jtag_end(0);
  jtag_read_cmd4->set_bitcount(32);
  jtag_read_cmd4->set_period(PERIOD);
  
  jtag_read_cmd5->set_jtag_start(4);
  jtag_read_cmd5->set_jtag_end(0);
  jtag_read_cmd5->set_bitcount(36);
  jtag_read_cmd5->set_period(PERIOD);
  jtag_read_cmd5->set_write_back(true);
 
  // WRITE COMMAND

  jtag_write_cmd1->set_jtag_start(4);
  jtag_write_cmd1->set_jtag_end(0);
  jtag_write_cmd1->set_bitcount(36);
  jtag_write_cmd1->set_period(PERIOD);
  jtag_write_cmd1->set_payload(0x2);
  jtag_write_cmd1->set_data(0);
  jtag_write_cmd1->set_data_offset(3);
  jtag_write_cmd1->set_data_length(8);
  jtag_write_cmd1->set_data_is_other(false);
  jtag_write_cmd1->set_data_is_address(true);

  jtag_write_cmd2->set_jtag_start(0);
  jtag_write_cmd2->set_jtag_end(0);
  jtag_write_cmd2->set_bitcount(35);
  jtag_write_cmd2->set_period(PERIOD);

  jtag_write_cmd3->set_jtag_start(4);
  jtag_write_cmd3->set_jtag_end(0);
  jtag_write_cmd3->set_bitcount(36);
  jtag_write_cmd3->set_period(PERIOD);
  jtag_write_cmd3->set_payload(0x6);
  jtag_write_cmd3->set_data_offset(3);
  jtag_write_cmd3->set_data_length(8);
  jtag_write_cmd3->set_data_is_other(true);
  jtag_write_cmd3->set_write_back(true);

  // RESET COMMAND

  jtag_reset_cmd1->set_jtag_start(0);
  jtag_reset_cmd1->set_jtag_end(0);
  jtag_reset_cmd1->set_bitcount(5);
  jtag_reset_cmd1->set_period(PERIOD);

  // Select DAPACC : 0xA
  jtag_reset_cmd2->set_jtag_start(0xB);
  jtag_reset_cmd2->set_jtag_end(1);
  jtag_reset_cmd2->set_bitcount(10);
  jtag_reset_cmd2->set_period(PERIOD);
  jtag_reset_cmd2->set_payload(0x2BF);

  // Reset JTag CLock Domain
  // Access to Control/Status at 0x4
  jtag_reset_cmd3->set_jtag_start(0x4);
  jtag_reset_cmd3->set_jtag_end(1);
  jtag_reset_cmd3->set_bitcount(36);
  jtag_reset_cmd3->set_period(PERIOD);
  jtag_reset_cmd3->set_payload(0x20000004);

  // Select DAPACC : 0xA
  jtag_reset_cmd4->set_jtag_start(0xB);
  jtag_reset_cmd4->set_jtag_end(1);
  jtag_reset_cmd4->set_bitcount(10);
  jtag_reset_cmd4->set_period(PERIOD);
  jtag_reset_cmd4->set_payload(0x2BF);

  // Select the JTAG Accecc Point and register within bank -> target specific 
  jtag_reset_cmd5->set_jtag_start(0x4);
  jtag_reset_cmd5->set_jtag_end(1);
  jtag_reset_cmd5->set_bitcount(36);
  jtag_reset_cmd5->set_period(PERIOD);
  jtag_reset_cmd5->set_payload(0x010000008);

  // Select APACC : 0XB
  jtag_reset_cmd6->set_jtag_start(0xB);
  jtag_reset_cmd6->set_jtag_end(1);
  jtag_reset_cmd6->set_bitcount(10);
  jtag_reset_cmd6->set_period(PERIOD);
  jtag_reset_cmd6->set_payload(0x2FF);

  jtag_reset_cmd7->set_jtag_start(4);
  jtag_reset_cmd7->set_jtag_end(1);
  jtag_reset_cmd7->set_bitcount(36);
  jtag_reset_cmd7->set_period(PERIOD);
  jtag_reset_cmd7->set_payload(0x800000120);
  //jtag_reset_cmd8->set_payload(0x800000120);

}

void build_lpc1850(PROTOCOLS* protocols) {

  PROTOCOL* protocol = protocols->add_protocols();

  protocol->set_target("lpc1850");

  //auto hl_command_read  = protocol->add_commands();
  auto hl_command_write = protocol->add_commands();
  auto hl_command_reset = protocol->add_commands();
  auto hl_command_read  = protocol->add_commands();

  hl_command_read->set_type(HL_COMMAND_CMD_TYPE_READ);
  hl_command_read->set_size(64*5);

  hl_command_write->set_type(HL_COMMAND_CMD_TYPE_WRITE);
  hl_command_write->set_size(64*3);
  
  hl_command_reset->set_type(HL_COMMAND_CMD_TYPE_RESET);
  hl_command_reset->set_size(64*5);

  auto jtag_read_cmd1 = hl_command_read->add_commands();
  auto jtag_read_cmd2 = hl_command_read->add_commands();
  auto jtag_read_cmd3 = hl_command_read->add_commands();
  auto jtag_read_cmd4 = hl_command_read->add_commands();
  auto jtag_read_cmd5 = hl_command_read->add_commands();
  auto jtag_write_cmd1 = hl_command_write->add_commands();
  auto jtag_write_cmd2 = hl_command_write->add_commands();
  auto jtag_write_cmd3 = hl_command_write->add_commands();
  auto jtag_reset_cmd1 = hl_command_reset->add_commands();
  auto jtag_reset_cmd2 = hl_command_reset->add_commands();
  auto jtag_reset_cmd3 = hl_command_reset->add_commands();
  auto jtag_reset_cmd4 = hl_command_reset->add_commands();
  auto jtag_reset_cmd5 = hl_command_reset->add_commands();

  // READ COMMAND

  jtag_read_cmd1->set_jtag_start(4);
  jtag_read_cmd1->set_jtag_end(1);
  jtag_read_cmd1->set_bitcount(35);
  jtag_read_cmd1->set_period(PERIOD);
  jtag_read_cmd1->set_payload(0x2);
  jtag_read_cmd1->set_data(0);
  jtag_read_cmd1->set_data_offset(3);
  jtag_read_cmd1->set_data_length(8);
  jtag_read_cmd1->set_data_is_address(true);

  jtag_read_cmd2->set_jtag_start(1);
  jtag_read_cmd2->set_jtag_end(1);
  jtag_read_cmd2->set_bitcount(32);
  jtag_read_cmd2->set_period(PERIOD);

  jtag_read_cmd3->set_jtag_start(4);
  jtag_read_cmd3->set_jtag_end(1);
  jtag_read_cmd3->set_bitcount(35);
  jtag_read_cmd3->set_period(PERIOD);
  jtag_read_cmd3->set_payload(0x100000007);
  jtag_read_cmd3->set_data(0);
  jtag_read_cmd3->set_data_offset(3);
  jtag_read_cmd3->set_data_length(9);
  jtag_read_cmd3->set_data_is_other(true);
  jtag_read_cmd3->set_data_is_address(false);

  jtag_read_cmd4->set_jtag_start(1);
  jtag_read_cmd4->set_jtag_end(1);
  jtag_read_cmd4->set_bitcount(32);
  jtag_read_cmd4->set_period(PERIOD);
  
  jtag_read_cmd5->set_jtag_start(4);
  jtag_read_cmd5->set_jtag_end(1);
  jtag_read_cmd5->set_bitcount(35);
  jtag_read_cmd5->set_period(PERIOD);
  jtag_read_cmd5->set_write_back(true);
  
  // WRITE COMMAND
  jtag_write_cmd1->set_jtag_start(4);
  jtag_write_cmd1->set_jtag_end(1);
  jtag_write_cmd1->set_bitcount(35);
  jtag_write_cmd1->set_period(PERIOD);
  jtag_write_cmd1->set_payload(0x2);
  jtag_write_cmd1->set_data(0);
  jtag_write_cmd1->set_data_offset(3);
  jtag_write_cmd1->set_data_length(8);
  jtag_write_cmd1->set_data_is_other(false);
  jtag_write_cmd1->set_data_is_address(true);

  jtag_write_cmd2->set_jtag_start(1);
  jtag_write_cmd2->set_jtag_end(1);
  jtag_write_cmd2->set_bitcount(5);
  jtag_write_cmd2->set_period(PERIOD);

  jtag_write_cmd3->set_jtag_start(4);
  jtag_write_cmd3->set_jtag_end(1);
  jtag_write_cmd3->set_bitcount(35);
  jtag_write_cmd3->set_period(PERIOD);
  jtag_write_cmd3->set_payload(0x6);
  jtag_write_cmd3->set_data_offset(3);
  jtag_write_cmd3->set_data_length(8);
  jtag_write_cmd3->set_data_is_other(true);

  // RESET COMMAND
  jtag_reset_cmd1->set_jtag_start(0);
  jtag_reset_cmd1->set_jtag_end(0);
  jtag_reset_cmd1->set_bitcount(1);
  jtag_reset_cmd1->set_period(PERIOD);
 
  jtag_reset_cmd2->set_jtag_start(0xB);
  jtag_reset_cmd2->set_jtag_end(4);
  jtag_reset_cmd2->set_bitcount(4);
  jtag_reset_cmd2->set_period(PERIOD);
  jtag_reset_cmd2->set_payload(0xA);
 
  jtag_reset_cmd3->set_jtag_start(0x4);
  jtag_reset_cmd3->set_jtag_end(1);
  jtag_reset_cmd3->set_bitcount(35);
  jtag_reset_cmd3->set_period(PERIOD);
  jtag_reset_cmd3->set_payload(0x280000002);
  
  jtag_reset_cmd4->set_jtag_start(0xB);
  jtag_reset_cmd4->set_jtag_end(4);
  jtag_reset_cmd4->set_bitcount(4);
  jtag_reset_cmd4->set_period(PERIOD);
  jtag_reset_cmd4->set_payload(0xB);
 
  jtag_reset_cmd5->set_jtag_start(0x4);
  jtag_reset_cmd5->set_jtag_end(0x1);
  jtag_reset_cmd5->set_bitcount(35);
  jtag_reset_cmd5->set_period(PERIOD);
  jtag_reset_cmd5->set_payload(0x110000010);

}
