#include"main.h"
#include"C_time.h"
#include"C_config.h"
#include"C_log.h"
#include"C_msg.h"
#include"C_data.h"
#include"C_cnt.h"
#include"C_jang.h"
#include"C_socket.h"

class C_main_handle
{
	private :
		struct sockaddr_in s_add;
		char **_key;
		char **_config_path;
		char _message[150];
		C_time _date_time;
		C_config _config;
		C_log _log;
		C_msg _msg;
		C_data _data;
		C_cnt _cnt;
		C_jang _jang;
		int _data_fd;
		int _jang_status;
		C_socket _socket;

	public :
		C_main_handle( int *argc, char *argv[]) : _key(&argv[1]), _config_path(&argv[2])
		{
			/* Argument Read, Check and Display */
			if(*argc != 3)
			{
				cout << "Parameter 수가 맞지 않는군요." << endl;
				cout << "Usage : " << *argv << " <WhisaCode+TR> <Config File>" << endl;
				exit(1);
			}
		}

		void F_get_date_time()
		{
			_date_time.F_update_date_time();
			//cout << _date_time.F_get_time() << endl;
			//cout << _date_time.F_get_date() << endl;
			//cout << _date_time.F_get_time_edit() << endl;
			//cout << _date_time.F_get_date_edit() << endl;
		}

		void F_get_config()
		{
			try
			{
				_config.F_read_config(*_key, *_config_path);
				//cout << _config.F_get_company_id() << endl; 
				//cout << _config.F_get_tr_code() << endl; 
				//cout << _config.F_get_communication_type() << endl;
				//cout << _config.F_get_process_name() << endl;
				//cout << _config.F_get_object_name() << endl; 
				//cout << _config.F_get_port_number() << endl;
				//cout << _config.F_get_ip_number() << endl;
				//cout << _config.F_get_tcp_process_name() << endl;
				//cout << _config.F_get_data_file_name() << endl; 
				//cout << _config.F_get_log_file_name() << endl; 
				//cout << _config.F_get_cnt_file_name() << endl;
				//cout << _config.F_get_jang_file_name() << endl;
				//cout << _config.F_get_cnt_gubun() << endl;
				//cout << _config.F_get_message_length() << endl;
				//cout << _config.F_get_msg_file_name() << endl;
				//cout << _config.F_get_header_tr_code() << endl;
			}
			catch(const char* _message)
			{
				cout << _message << endl;
				exit(1);
			}
		}

		void F_open_file()
		{
			try
			{
				/* 1. LOG File Open */
				_log.F_open_log_file(_config.F_get_process_name(), _config.F_get_log_file_name());
				/* 2. MSG File Open */
				_log.F_write_log(_msg.F_open_msg_file(_config.F_get_process_name(), _config.F_get_msg_file_name()));

				/* 3. Program Start Message */
				memset(_message, 0x00, sizeof(_message));
				sprintf(_message, "Interface PROGRAM STARTING                          ");
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);

				/* 4. Data File Open */
				_log.F_write_log(_data.F_open_data_file(_config.F_get_data_file_name()));
				_data_fd = _data.F_get_data_fd();

				/* 5. CNT File Open */
				_log.F_write_log(_cnt.F_open_cnt_file(_config.F_get_cnt_file_name(), _config.F_get_company_id(), _config.F_get_cnt_gubun()));

				/* 6. Jang File Open */
				_log.F_write_log(_jang.F_open_jang_file(_config.F_get_jang_file_name()));
			}
			catch(const char* _message)
			{
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);
				cout << _message << endl;
				exit(1);
			}
		}

		void F_get_cnt()
		{
			try
			{
				_cnt.F_read_cnt(_config.F_get_company_id(), _config.F_get_cnt_gubun());

				/* CNT Status Check */
				memset(_message, 0x00, sizeof(_message));
				sprintf(_message, "Last Count : %ld", _cnt.F_get_last_data_count());
				_log.F_write_log(_message);
				memset(_message, 0x00, sizeof(_message));
				sprintf(_message, "Proc Status : %d", _cnt.F_get_process_status());
				_log.F_write_log(_message);
				memset(_message, 0x00, sizeof(_message));
				sprintf(_message, "Link Status : %d", _cnt.F_get_link_status());
				_log.F_write_log(_message);
			}
			catch(const char* _message)
			{
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);
				cout << _message << endl;
				exit(1);
			}
		}

		void F_get_jang()
		{
			try
			{
				_jang.F_read_jang();
				_jang_status = _jang.F_get_jang_status();
			}
			catch(const char* _message)
			{
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);
				cout << _message << endl;
				exit(1);
			}
		}

		void F_temp()
		{
			try
			{
				_socket.F_create_socket(_config.F_get_ip_number(), _config.F_get_port_number());
			}
			catch(const char* _message)
			{
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);
				cout << _message << endl;
				exit(1);
			}
		}

		void F_stop_process(int option)
		{
			try
			{
				if(_cnt.F_put_process_stop(option) == SUCCESS)
				{
					memset(_message, 0x00, sizeof(_message));
					sprintf(_message, "Interface PROGRAM NORMAL STOP");
					_log.F_write_log(_message);
					_msg.F_write_msg(_message);
					exit(0);
				}
				else
				{
					memset(_message, 0x00, sizeof(_message));
					sprintf(_message, "Process Stop Error..");
					_log.F_write_log(_message);
					_msg.F_write_msg(_message);
					exit(1);
				}
			}
			catch(const int _FAIL)
			{
				memset(_message, 0x00, sizeof(_message));
				sprintf(_message, "Interface PROGRAM ABNORMAL STOP");
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);
				exit(1);
			}
			catch(const char* _message)
			{
				_log.F_write_log(_message);
				_msg.F_write_msg(_message);
				cout << _message << endl;
				exit(1);
			}
		}

};

int main(int argc, char *argv[])
{
	
	argc = 3;
	argv[1] = "999r1";
	argv[2] = "tconfig";

	/* Parameter Check */
	C_main_handle _control(&argc, argv);

	/* 사전 처리 */
	_control.F_get_date_time();			/* 현재 시스템 날짜와 시간으로 Update */
	_control.F_get_config();			/* CONFIG에서 정보 가져오기 */ 
	_control.F_open_file();				/* CONFIG에서 가져 온 정보로 관련파일들 Open */
	_control.F_get_cnt();				/* Process상태, Link여부, 마지막 데이터 개수 */
	_control.F_get_jang();				/* JANG File을 읽어와서 Variable Setting */
	//_control.F_stop_process(FAIL);
	//_control.F_stop_process(SUCCESS);
	_control.F_temp();

	return 0;
}