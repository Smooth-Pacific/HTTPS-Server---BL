#include <fstream>
#include <iostream>
using namespace std;

class Logging {
	private:
		ofstream m_file;
	public:
		Logging(string file) {
			try {
				m_file.open(file);
			} catch(...) {
				cerr << "file not found";
			}
		}
		~Logging() {
			m_file.close();
		}

		void log() {
			string text = "this is a log\n";
			m_file.write(text.data(), text.size());
		}
};