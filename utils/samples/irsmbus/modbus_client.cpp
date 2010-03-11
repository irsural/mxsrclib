#include <irscpp.h>
#include <irsstd.h>
#include <irsmbus.h>
#include <irsarcherror.h>
#include <irskbd.h>

mxip_t make_ip(irs_u8 a_ip0, irs_u8 a_ip1, irs_u8 a_ip2,
  irs_u8 a_ip3)
{
  mxip_t ip = {{a_ip0, a_ip1, a_ip2, a_ip3}};
  return ip;
}

int main()
{
  irs::gcc_linux::set_error_handler();
  irs::mlog().rdbuf(cout.rdbuf());

  //Размеры массивов указаны в байтах:
  size_t discret_inputs_size = 8192;
  size_t coils_size = 8192;
  size_t hold_registers_size = 65536;
  size_t input_registers_size = 65536;
  
  //Инициализация коммуникационного протокола, на котором будет работать клиент:
  static irs::hardflow::tcp_client_t tcp_hardflow(make_ip(127,0,0,1), 5006);
  
  irs::modbus_client_t modbus_client(&tcp_hardflow, 
    irs::mxdata_ext_t::mode_refresh_manual, discret_inputs_size, coils_size,
    hold_registers_size, input_registers_size);

  size_t sum_size = discret_inputs_size + coils_size + hold_registers_size*2 +
    input_registers_size*2;

  irs::raw_data_t<irs_u8> write_buf(sum_size);
  for(size_t buf_idx = 0; buf_idx < sum_size; buf_idx ++)
  {
    write_buf[buf_idx] = buf_idx + 10;
  }
  irs::raw_data_t<irs_u8> read_buf(sum_size);

  irs::loop_timer_t loop_timer(irs::make_cnt_s(3));

  enum mode_t{
    first_read_mode,
    write_mode,
    check_read_mode
  };
  mode_t mode = first_read_mode;

  static irs::gcc_linux::keyboard_t keyboard;

  for(;;)
  {
    modbus_client.tick();
    if(loop_timer.check())
    {
      #ifdef NOP //Автоматический режим
      if(modbus_client.connected()) {
        switch(mode)
        {
          case first_read_mode:
          {
            if(modbus_client.status() == irs::mxdata_ext_t::status_completed) {
              modbus_client.read(read_buf.data(), discret_inputs_size + 4, 1);
              irs::mlog() << "data before write = " <<
                int(read_buf[0]) << endl;
              mode = write_mode;
            }
          }
          break;
          case write_mode:
          {
            if(modbus_client.status() == irs::mxdata_ext_t::status_completed) {
              modbus_client.write(write_buf.data(), discret_inputs_size + 4, 1);
              irs::mlog() << "write data = " <<
                int(write_buf[discret_inputs_size + 4]) << endl;
              mode = check_read_mode;
            }
          }
          break;
          case check_read_mode:
          {
            if(modbus_client.status() == irs::mxdata_ext_t::status_completed) {
              modbus_client.read(read_buf.data(), discret_inputs_size + 4, 1);
              irs::mlog() << "data after write = " <<
                int(read_buf[0]) << endl;
            }
          }
          break;
        }
      }
      #endif //NOP
      
      #ifndef NOP //Режим ручного управления
      if(modbus_client.connected()) {
        switch(mode)
        {
          case first_read_mode:
          {
            if(modbus_client.status() == irs::mxdata_ext_t::status_completed) {
              irs::mlog() << " first_read_mode" << endl;
              modbus_client.mark_to_recieve(discret_inputs_size + 4, 1);
              modbus_client.update();
              mode = write_mode;
            }
          }
          break;
          case write_mode:
          {
            if(modbus_client.status() == irs::mxdata_ext_t::status_completed) {
              modbus_client.read(read_buf.data(), discret_inputs_size + 4, 1);
              irs::mlog() << " write_mode" << endl;
              irs::mlog() << "data before write = " <<
                int(read_buf[0]) << endl;
              modbus_client.write(write_buf.data(), discret_inputs_size + 4, 1);
              modbus_client.mark_to_send(discret_inputs_size + 4, 1);
              irs::mlog() << "write data = " <<
                int(write_buf[discret_inputs_size + 4]) << endl;
              modbus_client.mark_to_recieve(discret_inputs_size + 4, 1);
              modbus_client.update();
              mode = check_read_mode;
            }
          }
          break;
          case check_read_mode:
          {
            if(modbus_client.status() == irs::mxdata_ext_t::status_completed) {
              irs::mlog() << " check_read_mode" << endl;
              modbus_client.read(read_buf.data(), discret_inputs_size + 4, 1);
              irs::mlog() << "data after write = " <<
                int(read_buf[0]) << endl;
              modbus_client.update();
            }
          }
          break;
        }
      }
      #endif //NOP
    }
    if(keyboard.key() == irs::gcc_linux::keyboard_t::key_escape)
      break;
  }
}