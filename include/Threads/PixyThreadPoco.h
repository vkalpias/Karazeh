/*
 *  Copyright (c) 2011 Ahmad Amireh <ahmad@amireh.net>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#ifndef H_PixyThreadPoco_H
#define H_PixyThreadPoco_H

#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Patcher.h"

namespace Pixy {

  class Worker : public Poco::Runnable {
    public:

    Worker() {}
    ~Worker() {}

    void run() {
      Patcher::getSingleton()();
    }
  };

  template <class T>
  class Thread {
    public:
    Thread() {};

    Thread(T& inT) {
      mPocoThread.start(mWorker);
    };

    Thread(T* inT) {

    };

    virtual ~Thread() {
    };

    private:
    Poco::Thread mPocoThread;
    Worker mWorker;

    Thread(const Thread& rhs);
    Thread& operator=(const Thread& rhs);
  };

}

#endif // H_PixyThreadPoco_H
