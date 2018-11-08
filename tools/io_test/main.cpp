#include "api.h"

using namespace std;

int main(int argc, char* argv[]) {


  device* io = init();

  reset(io);

  uint32_t address=0x20000000, size=0x4;

  uint8_t wdata[] = {0xAB, 0xAB, 0xAB, 0xAB};


  for(auto i=0; i<1; i++) {
    wdata[0] += i;
    wdata[1] += i;
    wdata[2] += i;
    wdata[3] += i;

    write(io, address, size, wdata);

    auto data = read(io, address, size);
    delete data;

    address += 4;

    if(memcmp(data, wdata, size) != 0) {
	cout << "Test "<<i<<" failed...";
        cout << "    Expecting 0x"<<std::hex<<(uint32_t*)wdata;
	cout << " but get 0x"<<std::hex<<(uint32_t*)data;
	cout << " at 0x"<<std::hex<<address;
    }
  }

}
