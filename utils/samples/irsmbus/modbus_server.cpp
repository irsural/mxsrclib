#include <irscpp.h>
#include <irsstd.h>
#include <irsmbus.h>
#include <irsarcherror.h>
#include <irskbd.h>

int main()
{
  irs::gcc_linux::set_error_handler();
  irs::mlog().rdbuf(cout.rdbuf());

  //–азмеры массивов указаны в байтах:
  size_t discret_inputs_size = 8192;
  size_t coils_size = 8192;
  size_t hold_registers_size = 65536;
  size_t input_registers_size = 65536;

  //»нициализаци€ коммуникационного протокола, на котором будет работать сервер:
  static irs::hardflow::tcp_server_t tcp_hardflow(5006);

  irs::modbus_server_t modbus_server(&tcp_hardflow, 
    discret_inputs_size, coils_size, hold_registers_size, input_registers_size);

  size_t sum_size = discret_inputs_size + coils_size + hold_registers_size*2 +
    input_registers_size*2;

  irs::raw_data_t<irs_u8> write_buf(sum_size);
  for(size_t buf_idx = 0; buf_idx < sum_size; buf_idx ++)
  {
    write_buf[buf_idx] = buf_idx + 10;
  }
  modbus_server.write(write_buf.data(), 0, sum_size);

  static irs::gcc_linux::keyboard_t keyboard;
  
  for(;;)
  {
    modbus_server.tick();
    if(keyboard.key() == irs::gcc_linux::keyboard_t::key_escape)
      break;
  }
}
