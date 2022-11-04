#include "Webgame.hpp"
#include <future>
#include <memory>

class Game {
  public:
    Game() {}
};

class GameHandler
{
  public:
	  GameHandler(Webconsole *ct, std::shared_ptr<Game> game) {}
	  void onmessage(std::string sInput,std::map<
        decltype(websocketpp::lib::weak_ptr<void>().lock().get()),
        GameHandler*> *ptr, bool& global_shutdown) {
	  }
};

int main()
{
  Webgame<GameHandler, Game> gl(std::make_shared<Game>());
  gl.run(9001);
  return 0;
}
