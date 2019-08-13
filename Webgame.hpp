#include <websocketpp/config/asio_no_tls.hpp>

#include <websocketpp/server.hpp>

#include <iostream>
#include <mutex>
#include "Webconsole.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg);

template<typename GameType>
class Webgame
{
        public:
                std::mutex ml;
                std::map<decltype(websocketpp::lib::weak_ptr<void>().lock().get()),GameType*> mp;
                server echo_server;

                Webgame()
                {}

                void run()
                {

                        try {
                                // Set logging settings
                                echo_server.set_access_channels(websocketpp::log::alevel::all);
                                echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

                                // Initialize Asio
                                echo_server.init_asio();

                                // Register our message handler
                                echo_server.set_message_handler(bind(&Webgame<GameType>::on_message,this,&echo_server,::_1,::_2));
                                echo_server.set_open_handler(bind(&Webgame<GameType>::on_open,this,::_1));
                                echo_server.set_close_handler(bind(&Webgame<GameType>::on_open,this,::_1));

                                // Listen on port 9002
                                echo_server.listen(9002);

                                // Start the server accept loop
                                echo_server.start_accept();

                                // Start the ASIO io_service run loop
                                echo_server.run();
                        } catch (websocketpp::exception const & e) {
                                std::cout << e.what() << std::endl;
                        } catch (...) {
                                std::cout << "other exception" << std::endl;
                        }

                }

        void on_close(websocketpp::connection_hdl hdl)
        {
               std::lock_guard lk(ml);
                try
                {
                        auto k = mp.at(hdl.lock().get());
                        delete k;
                        if(mp.size()>1)
                                mp.erase(hdl.lock().get());
                        else
                                mp.clear();
                }
                catch(...)
                {
                }
        }

        void on_open(websocketpp::connection_hdl hdl)
        {
                {
                        std::lock_guard lk(ml);
                try
                {
                        auto k = mp.at(hdl.lock().get());
                        return;
                }
                catch(...)
                {
                        mp[hdl.lock().get()] = new GameType(new Webconsole(&echo_server,hdl));
                }
                }
        }

        void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
                {
                std::lock_guard lk(ml);
                try
                {
                        auto k = mp.at(hdl.lock().get());
                        k->onmessage(msg->get_payload());
                }
                catch(...)
                {
                        return;
                }
        }
}
};
