from threading import Lock, Thread
from random import randint

___author___ = "Joakim Sjøhaug"

class Node():

    def __init__(self, key):
        self.key = key
        self.next = None
        self.marked = False
        self.lock = Lock()

    def __str__(self):
        return f"Node value: {self.key}"

class FineGrainedList():
    def __init__(self):
        self.root = None
        self.lock = Lock()
        self.head = Node(None)

    def add(self, key):
        node = Node(key)

        # Acquire head
        self.head.lock.acquire()
        prev = self.head

        if self.root is None:
            self.root = node
            self.head.next = self.root
            prev.lock.release()
            return
        
        self.root.lock.acquire()

        # New root
        if self.root.key > key:
            node.next = self.root
            self.root = node
            self.root.next.lock.release()
            self.head.next = self.root
            prev.lock.release()
            return
        elif key == self.root.key:
            self.root.lock.release()
            prev.lock.release()
            return

        current = prev.next

        try:
            while current is not None:
                if current.key > node.key:
                    break
                elif current.key == node.key:
                    prev.lock.release()
                    current.lock.release()
                    return
            
                prev.lock.release()
                prev = current
                current = current.next
                current.lock.acquire()
        except Exception as e:
            # Fails, then we are at the end
            prev.next = node
            prev.lock.release()
            return
        
        if prev.next is None:
            prev.next = node
        else:
            node.next = current
            prev.next = node
        
        prev.lock.release()
        current.lock.release()
        
    
    def remove(self, key):
        
        prev = self.head
        prev.lock.acquire()
        
        if self.root is None:
            prev.lock.release()
            return
        
        self.root.lock.acquire()

        if self.root.next is None:
            if self.root.key == key:
                self.root.lock.release()
                self.root = None 

            self.head.next = None
            prev.lock.release()
            return
        
        current = prev.next

        try: 
            while current is not None:
                if current.key == key:
                    break
                
                prev.lock.release()
                prev = current
                current = current.next
                current.lock.acquire()
        except Exception as e:
            # End of list
            prev.lock.release()
            return
        
        prev.next = current.next
        prev.lock.release()
        current.lock.release()
        
    def __str__(self):
        current = self.root
        list_string = ""

        while current is not None:
            list_string += str(current) + "\n"
            current = current.next
        
        return list_string


myList = FineGrainedList()

def test_conncurrent_add(pid):
    numbers = [randint(0, 1000) for _ in range(100)]
    for number in numbers:
        myList.add(number)

def test_conncurrent_remove(pid):
    numbers = [randint(0, 1000) for _ in range(10)]
    for number in numbers:
        myList.remove(number)

if __name__ == "__main__":
    threads = []
    
    for i in range(10):
        if i > 7:
            t = Thread(target=test_conncurrent_remove, args=(i,))
            threads.append(t)
        else:
            t = Thread(target=test_conncurrent_add, args=(i,))
            threads.append(t)

    for thread in threads:
        thread.start()

    print(myList)
    print(___author___)
