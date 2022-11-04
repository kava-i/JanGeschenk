#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <signal.h>
#include "Webconsole.hpp"


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg);

static inline void *gWebgameAddr = nullptr;
namespace asio = websocketpp::lib::asio;
#ifdef _COMPILE_FOR_SERVER_
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;
#endif

template<typename T>
void sig_handler(int) {
  delete (T)gWebgameAddr;
}

inline std::string get_password() {
  return "password";
}

template<typename GameType, typename Game>
class Webgame
{
  public:
    std::mutex ml_;
    std::map<decltype(websocketpp::lib::weak_ptr<void>().lock().get()),GameType*> user_connections_;
    server echo_server;
    bool global_shutdown;
    bool wait;
    std::shared_ptr<Game> game_;

    Webgame(std::shared_ptr<Game> game) {
      signal(SIGTERM, sig_handler<Webgame<GameType, Game>*>);
      global_shutdown=false;
      wait=true;
      game_ = game;
    }

    ~Webgame() {
      echo_server.stop();
    }

    #ifdef _COMPILE_FOR_SERVER_
	  context_ptr on_tls_init(websocketpp::connection_hdl hdl) {
	    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>
        (asio::ssl::context::sslv23);
	    try {
		    ctx->set_options(asio::ssl::context::default_workarounds |
			    asio::ssl::context::no_sslv2 |
			    asio::ssl::context::no_sslv3 |
			    asio::ssl::context::no_tlsv1 |
			    asio::ssl::context::single_dh_use);
        std::cout << "About to use key at: /etc/letsencrypt/live/kava-i.de/fullchain.pem" << std::endl;
		    ctx->set_password_callback(bind(&get_password));
        ctx->use_certificate_chain_file("/etc/letsencrypt/live/kava-i.de/fullchain.pem");
        ctx->use_private_key_file("/etc/letsencrypt/live/kava-i.de/privkey.pem", 
            asio::ssl::context::pem);
        ctx->use_tmp_dh_file("/etc/letsencrypt/ssl-dhparams.pem");
        // Example method of generating this file:
        // `openssl dhparam -out dh.pem 2048`
        // Mozilla Intermediate suggests 1024 as the minimum size to use
        // Mozilla Modern suggests 2048 as the minimum size to use.

        std::string ciphers;
        ciphers = "ECDHE-ECDSA-CHACHA20-POLY1305:ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA:ECDHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA256:DHE-RSA-AES256-SHA:ECDHE-ECDSA-DES-CBC3-SHA:ECDHE-RSA-DES-CBC3-SHA:EDH-RSA-DES-CBC3-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:AES128-SHA256:AES256-SHA256:AES128-SHA:AES256-SHA:DES-CBC3-SHA:!DSS";
        if (SSL_CTX_set_cipher_list(ctx->native_handle() , ciphers.c_str()) != 1) {
            std::cout << "Error setting cipher list" << std::endl;
        }
      } 
      catch (std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
      }
      return ctx;
	  }
    #endif

    void stop() {
      echo_server.stop();
    }

	  void run(int port) {
	    try {
        // Set logging settings
        echo_server.set_access_channels(websocketpp::log::alevel::all);
        echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        echo_server.init_asio();

        // Register our message handler
        echo_server.set_message_handler(bind(&Webgame<GameType, Game>::on_message,this,&echo_server,::_1,::_2));
        #ifdef _COMPILE_FOR_SERVER_
          echo_server.set_tls_init_handler(bind(&Webgame<GameType>::on_tls_init,this,::_1));
        #endif
        echo_server.set_open_handler(bind(&Webgame<GameType, Game>::on_open,this,::_1));
        echo_server.set_close_handler(bind(&Webgame<GameType, Game>::on_close,this,::_1));
        echo_server.set_reuse_addr(true);

        // Listen on port 9002
        echo_server.listen(port);

        // Start the server accept loop
        echo_server.start_accept();

        // Start the ASIO io_service run loop
        echo_server.run();
        std::cout << "Webgame started on port " << port << std::endl;
      } 
      catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
      } 
      catch (...) {
        std::cout << "other exception" << std::endl;
      }
	  }

    void on_close(websocketpp::connection_hdl hdl) {
      std::lock_guard lk(ml_);
      try {
        auto k = user_connections_.at(hdl.lock().get());
        delete k;
        if(user_connections_.size()>1)
          user_connections_.erase(hdl.lock().get());
        else
          user_connections_.clear();
      }
      catch(...) {}
    }

    void on_open(websocketpp::connection_hdl hdl) {
      std::lock_guard lk(ml_);
      try {
        user_connections_.at(hdl.lock().get());
        return;
      }
      catch(...) {
        user_connections_[hdl.lock().get()] = new GameType(new Webconsole(&echo_server,hdl), game_);
      }
    }

    void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
      std::lock_guard lk(ml_);
      try {
        auto game = user_connections_.at(hdl.lock().get());
        game->onmessage(msg->get_payload(),&user_connections_, global_shutdown);
        if (global_shutdown == true && wait == true)
          wait = false;
        else if (global_shutdown == true && wait == false)
          echo_server.stop();
      }
      catch(...) {
        return;
      }
    }
};
