#include "Webgame.hpp"

class game
{
  public:
	  game(Webconsole *ct) {}
	  void onmessage(std::string sInput,std::map<
        decltype(websocketpp::lib::weak_ptr<void>().lock().get()),
        game*> *ptr, bool& global_shutdown) {
	  }
};

int main()
{
    Webgame<game> gl;
    gl.run(9001);
    return 0;
}
