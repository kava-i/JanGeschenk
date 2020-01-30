#include "Webgame.hpp"

class game
{
    public:
	game(Webconsole *ct)
	{}
	void onmessage(std::string sInput,std::map<decltype(websocketpp::lib::weak_ptr<void>().lock().get()),game*> *ptr)
	{
	}
};

int main()
{
    Webgame<game> gl;
    gl.run();
    return 0;
}
