from cmd import Cmd

class IllegalRegisterError(Exception):

    def __init__(self):
        super(IllegalRegisterError, self).__init__(self, "No such register!", None)


class MemoryFullError(Exception):

    def __init__(self):
        super(MemoryFullError, self).__init__(self, "Memory full!", None)


class NotInMemoryError(Exception):

    def __init__(self):
        super(NotInMemoryError, self).__init__(self, "Variable not in memory!", None)

class Processor(Cmd):

    memsize = 16
    memset = {}
    regset = {"R1": "Null", "R2": "Null", "R3": "Null", "R4": "Null", "AC": "Null"}


    def do_print(self, args):
        """
Prints the value of specified register or variable
Syntax :
        print [location]
[location] can be either a variable name or register number
        """
        if len(args.split(" ")) != 1:
            print("[ERROR]Wrong syntax for function 'print' : %s!" % args)
        else:
            self.perform_print(args)


    def do_load(self, args):
        """
Loads a value in register.
Syntax :
        load [source], [destination]
[source] can be either a variable name or a register number
[destination] should must be a register number
        """
        if len(args)==0:
            print("[ERROR] Specify a source and a destination!")
        else:
            address = args.split(", ")
            if len(address) != 2:
                print("[ERROR] Wrong syntax for `load`!")
            else:
                try:
                    self.perform_load(address[0], address[1])
                except IllegalRegisterError:
                    print("[ERROR] Bad register number %s " % address[1])
                except NotInMemoryError:
                    print("[ERROR] No such variable in memory")


    def do_store(self, args):
        """
Stores the value from a register to memory
Syntax :
        store [source], [destination]
[source] must be a register
[destination] can be either a variable name or register
If no such variable exists in memory, a new one will be created implicitly
        """
        if len(args) == 0:
            print("[ERROR] Specify a source and destination!")
        else:
            address = args.split(", ")
            if len(address) != 2:
                print("[ERROR] Wrong syntax for `store`!")
            else:
                try:
                    self.perform_store(address[0], address[1])
                except IllegalRegisterError:
                    print("[ERROR] Bad register number %s!" % address[0])
                except ValueError:
                    print("[ERROR] Bad variable name %s!" % address[1])
                except MemoryFullError:
                    print("[ERROR] Memory full!")
                
                
    def do_let(self, args):
        """
Stores the specified value in a variable in memory
Syntax :
        let [value], [destination]
[value] can be any value
[destination] must be variable
If the variable exists, that will be replaced.
Otherwise, a new variable will be created.
        """
        if len(args) == 0:
            print("[ERROR] Specify a value to store!")
        else:
            address = args.split(", ")
            if len(address) != 2:
                print("[ERROR] Wrong syntax for `let`")
            else:
                try:
                    self.perform_let(address[0], address[1])
                except MemoryFullError:
                    print("[ERROR] Memory full")
                except ValueError:
                    print("[ERROR] Bad variable name %s " % address[1])


    def do_unlet(self, args):
        """
Unloads a variable from memory.
Syntax :
        unlet [variable]
[variable] must be a variable in memory
Error will be raised if no such variable exists in memory.
        """
        if len(args) == 0:
            print("[ERROR] Wrong syntax for `unlet`")
        else:
            try:
                self.perform_unlet(args)
            except NotInMemoryError:
                print("[ERROR] No such variable exists in memory!")

    
    def do_incr(self, args):
        """
Increments the value of a variable by unity.
Syntax :
        incr [destination]
[destination] can be either a variable of a register address
        """
        if len(args) == 0:
            print("[ERROR] Wrong syntax for `incr`")
        else:
            try:
                self.perform_incr(args)
            except ValueError:
                print("[ERROR] Value stored in %s is not a number!" % args)
            except NotInMemoryError:
                print("[ERROR] No such variable exists in memory!")


    def do_quit(self, args):
        """Quits the system"""
        print("[INFO]Shutting down..")
        raise SystemExit


    def checkName(self, x):
        if x[0].isalpha() or x[0]=="_":
            for i in range(1, len(x)):
                if x[i].isalnum() or x[i] == "_":
                    pass
                else:
                    raise ValueError

        else:
            raise ValueError


    def perform_print(self, args):
        if args in self.regset:
            print("[INFO] Value of register "+args+" is "+str(self.regset[args]))
        elif args in self.memset:
            print("[INFO] Value of variable "+args+" is "+str(self.memset[args]))
        else:
            print("[ERROR] "+args+" is not a variable or register!")


    def perform_load(self, source, dest):
        if dest in self.regset:
            if source in self.regset:
                self.regset[dest] = self.regset[source]
            elif source in self.memset:
                self.regset[dest] = self.memset[source]
            else:
                raise NotInMemoryError
        else: 
            raise IllegalRegisterError



    def perform_store(self, source, dest):
        if source in self.regset:
            if dest in self.regset:
                self.regset[dest] = self.regset[source]
            else:
                self.perform_let(self.regset[source], dest)
        else:
            raise IllegalRegisterError


    def perform_let(self, value, dest):
        if dest in self.memset:
            self.memset[dest] = value
        elif len(self.memset) < self.memsize:
            self.checkName(dest)
            self.memset[dest] = value
        else:
            raise MemoryFullError


    def perform_unlet(self, dest):
        if dest in self.memset:
            del self.memset[dest]
        else:
            raise NotInMemoryError


    def perform_incr(self, dest):
        if dest in self.memset:
            self.perform_load(dest, "AC")
            self.perform_incr("AC")
            self.perform_store("AC", dest)
        elif dest in self.regset:
            self.regset[dest] = float(self.regset[dest]) + 1
        else:
            raise NotInMemoryError


console = Processor()
console.prompt = " > "
console.cmdloop("Welcome to Machine!")

