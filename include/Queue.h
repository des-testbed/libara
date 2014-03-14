#ifndef QUEUE_H_
#define QUEUE_H_

#include "ARAMacros.h"

#include <mutex>
#include <condition_variable>

ARA_NAMESPACE_BEGIN

template<typename T>
class Queue {
    private:
	struct Node {
            std::shared_ptr<T> data;
	    std::unique_ptr<Node> next;
	};


    public:        
        Queue():head(new Node),tail(head.get()){}
        Queue(const Queue& other) = delete;
        Queue& operator=(const Queue& other) = delete;

        /* */
        std::shared_ptr<T> tryPop(T& value) {
            std::unique_ptr<Node> const oldHead = tryPopHead(value);
	    return oldHead->data;
        }
       

        std::shared_ptr<T> tryPop(){
            std::unique_ptr<Node> oldHead = tryPopHead();

	    if (oldHead) {
                return oldHead->data;
            }

	    return std::shared_ptr<T>();
	}

	bool empty() {
	    std::lock_guard<std::mutex> headLock(headMutex);
	    return (head.get() == getTail());
	}

	void push(T value) {
	    std::shared_ptr<T> newData(std::make_shared<T>(std::move(value)));

            std::unique_ptr<Node> p(new Node);
            {
                std::lock_guard<std::mutex> tail_lock(tailMutex);
                tail->data = newData;
                Node* const newTail = p.get();
                tail->next = std::move(p);
                tail = newTail;
            }

            conditionVariable.notify_one();
        }

        Node* getTail() {
            std::lock_guard<std::mutex> tail_lock(tailMutex);
            return tail;
        }


    private:
	std::unique_ptr<Node> popHead() {
            std::unique_ptr<Node> oldHead = std::move(head);
            head = std::move(oldHead->next);
            return oldHead;
        }

	std::unique_ptr<Node> tryPopHead() {
            std::lock_guard<std::mutex> head_lock(headMutex);

	    if (head.get()==getTail() ) {
		return std::unique_ptr<Node>();
	    }

	    return popHead();
	}

	std::unique_ptr<Node> tryPopHead(T& value) {
	    std::lock_guard<std::mutex> headLock(headMutex);

	    if (head.get()==getTail()) {
		return std::unique_ptr<Node>();
	    }

	    value = std::move(*head->data);
	    return popHead();
	}

	std::mutex headMutex;
	std::unique_ptr<Node> head;
	std::mutex tailMutex;
        std::condition_variable conditionVariable;

        Node* tail;

};

ARA_NAMESPACE_END

#endif
