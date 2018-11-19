class Node():
    def __init__(self, key):
        self.key = key
        self.next = None

    def __str__(self):
        return f"Node value: {self.key}"

class CoarseGrainedList():
    def __init__(self):
        self.root = None
        self.lock = Lock()

    def add(self, key):
        node = Node(key)
        
        # Acquire lock 
        self.lock.acquire()
        
        if self.root is None:
            self.root = node
            self.lock.release()
            return

        # new root
        if self.root.key > key:
            node.next = self.root
            self.root = node
            self.lock.release()
            return

        current = self.root
        prev = current
       
        while current is not None:
            if current.key > key:
                break
            elif current.key == key:
                self.lock.release()
                return

            prev = current
            current = current.next

        if prev.next is None:
            node.next = None
            prev.next = node
        else:
            node.next = prev.next 
            prev.next = node
        
        self.lock.release()
        return
    
    def remove(self, key):
        current = self.root
        prev = None 
        self.lock.acquire()

        while current is not None:
            if current.key == key:
                break
            
            prev = current
            current = current.next

        if current is self.root: 
            self.root = current.next
        elif current is not None and prev is not None:
            prev.next = current.next

        self.lock.release()
        return
        
    def __str__(self):
        current = self.root
        list_string = ""

        while current is not None:
            list_string += str(current) + "\n"
            current = current.next
        
        return list_string


myList = CoarseGrainedList()

def test_conncurrent_add(pid):
    numbers = [randint(0, 1000) for _ in range(100)]
    #numbers = [897, 119, 897]
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
