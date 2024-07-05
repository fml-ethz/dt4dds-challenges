#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP

#include <iostream>
#include <time.h>

namespace progressbar {

    class ProgressBar {
        private:
            int _total;
            int _current;
            int _width;
            bool _finished = false;
            time_t _lastupdate;
            time_t _now;
            std::string _label;

        public:
            ProgressBar(int total, std::string label = "", int width = 50) : _total(total), _current(0), _width(width), _label(label) {
                std::cout << _label << ": [" << std::string(_width, ' ') << "] 0% 0/" << _total << std::flush;
                time(&_lastupdate);
            }

            ~ProgressBar() {
                if (!_finished) {
                    close();
                }
            }

            void update(int current) {
                time(&_now);
                if (current == _total || difftime(_now, _lastupdate) > 0.0) { // update every second
                    _lastupdate = _now;
                } else {
                    return;
                }
                _current = current;
                int progress = (int)((float)_current / _total * _width);
                std::cout << "\r" << _label << ": [" << std::string(progress, '=') << std::string(_width - progress, ' ') << "] " << (int)((float)_current / _total * 100) << "% " << _current << "/" << _total << std::flush;
            }

            void close() {
                if (!_finished) {
                    std::cout << std::endl;
                    _finished = true;
                }
                
            }

            void finish() {
                std::cout << "\r[" << std::string(_width, '=') << "] 100% " << _total << "/" << _total  << std::endl;
            }
    };

}


#endif