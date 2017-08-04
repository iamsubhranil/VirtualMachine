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


class UndefinedConditionError(Exception):

    def __init__(self):
        super(UndefinedConditionError, self).__init__(self, "Condition not defined!", None)


class NotInWhileError(Exception):

    def __init__(self):
        super(NotInWhileError, self).__init__(self, "No in a while loop!", None)


class Processor(Cmd):

    memsize = 16
    memset = {}
    regset = {"R1": "Null", "R2": "Null", "R3": "Null", "R4": "Null", "AC": "Null"}
    program_stack = []
    condition_stack = []
    stackpointer = 0
    inwhile = 0
    defined_conds = ["lt", "gt", "lte", "gte", "eq", "neq"]


    def postcmd(self, stop, line):
        # print("SP : %d" % self.stackpointer)
        if self.stackpointer != len(self.program_stack):
            self.cmdqueue.append(self.program_stack[self.stackpointer][0].__name__.replace("do_", "", 1)+" "+self.program_stack[self.stackpointer][1])
        return Cmd.postcmd(self, stop, line)


    def push_ps(self, args):
        if self.stackpointer == len(self.program_stack):
            self.program_stack.append(args)
            # print("Pushing to stack %s" % str(args))
        self.stackpointer += 1


    def default(self, line):
        print("[ERROR] Command not recognised! See `help`.")
        return 0

    def do_help(self, args):
        """
Shows help on the command usage.
Syntax :
        help [command]
[command] can be any of the available commands
        """
        self.push_ps([self.do_help, args])
        return Cmd.do_help(self, args)


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
            try:
                self.perform_print(args)
                self.push_ps([self.do_print, args])
            except NotInMemoryError:
                print("[ERROR] No such variable in memory")


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
                    self.push_ps([self.do_load, args])
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
                    self.push_ps([self.do_store, args])
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
                    self.push_ps([self.do_let, args])
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
                self.push_ps([self.do_unlet, args])
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
                self.push_ps([self.do_incr, args])
            except ValueError:
                print("[ERROR] Value stored in %s is not a number!" % args)
            except NotInMemoryError:
                print("[ERROR] No such variable exists in memory!")


    def do_decr(self, args):
        """
Decrements the value of a variable by unity.
Syntax :
        incr [destination]
[destination] can be either a variable of a register address
        """
        if len(args) == 0:
            print("[ERROR] Wrong syntax for `decr`")
        else:
            try:
                self.perform_decr(args)
                self.push_ps([self.do_decr, args])
            except ValueError:
                print("[ERROR] Value stored in %s is not a number!" % args)
            except NotInMemoryError:
                print("[ERROR] No such variable exists in memory!")
    
    
    def do_while(self, args):
        """
Execute a block of statements repeatedly based on a condition.
Syntax :
        while [cond], [var], [var/val]
[cond] will be one of
        1. lt : less than
        2. gt : greater than
        3. lte : less than or equal to
        4. gte : greater than or equal to
        5. eq : equal to
        6. ne : not equal to
[var] can be either of a memory variable or a register number
[var/val] can be either of a constant, memory variable or register number
Each `while` statement must be completed with a `endwhile` statement. Only when the `endwhile` statement will be encountered, the statements between the `while-endwhile` block will be executed continuously until the condition is met.
        """
        if len(args) < 0:
            print("[ERROR] Wrong syntax for `while`!")
        else:
            conds = args.split(", ")
            if len(conds) != 3:
                print("[ERROR] Wrong number of arguments for while")
            else:
                try:
                    self.perform_while(conds[0], conds[1], conds[2])
                    self.push_ps([self.do_while, args])
                except ValueError:
                    print("[ERROR] Both arguments must be numeric!")
                except UndefinedConditionError:
                    print("[ERROR] Condition is not defined!")
                except NotInMemoryError:
                    print("[ERROR] Variable is not in memory!")


    def do_endwhile(self, args):
        """
Denotes the termination and starts a while loop.
Syntax :
        endwhile
This function does not take any arguments.
        """
        if len(args) > 0:
            print("[ERROR] Wrong syntax for `endwhile`")
        else:
            try:
                self.perform_endwhile(args)
            except NotInWhileError:
                print("[ERROR] Not in a while loop!")
    
    
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
            raise NotInMemoryError


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


    def perform_decr(self, dest):
        if dest in self.memset:
            self.perform_load(dest, "AC")
            self.perform_decr("AC")
            self.perform_store("AC", dest)
        elif dest in self.regset:
            self.regset[dest] = float(self.regset[dest]) - 1
        else:
            raise NotInMemoryError


    def perform_while(self, cond, source, dest):
        if cond not in self.defined_conds:
            raise UndefinedConditionError
        elif (source not in self.memset) and (source not in self.regset):
            raise NotInMemoryError
        else:
            if source in self.regset:
                float(self.regset[source])
            else:
                float(self.memset[source])
            if dest in self.regset:
                float(self.regset[dest])
            elif dest in self.memset:
                float(self.memset[dest])
            else:
                float(dest)
            self.condition_stack.insert(0, [self.stackpointer, cond, source, dest])
            self.inwhile += 1


    def getval(self, arg):
        if arg in self.regset:
            return float(self.regset[arg])
        elif arg in self.memset:
            return float(self.memset[arg])
        else:
            return float(arg)


    def perform_endwhile(self, args):
        if self.inwhile == 0:
            raise NotInWhileError
        else:
            self.push_ps([self.do_endwhile, args])
            arg = self.condition_stack[0]
            lastwhile = arg[0]
            cond = arg[1]
            source = arg[2]
            dest = arg[3]
            getval = self.getval
            if (cond == "eq" and getval(source) == getval(dest)) \
            or (cond == "neq" and getval(source) != getval(dest)) \
            or (cond == "lt" and getval(source) < getval(dest)) \
            or (cond == "gt" and getval(source) > getval(dest)) \
            or (cond == "lte" and getval(source) <= getval(dest)) \
            or (cond == "gte" and getval(source) >= getval(dest)):
                self.stackpointer = lastwhile+1
            else:
                # self.stackpointer += 1
                self.inwhile -= 1
                self.condition_stack.pop(0)
                # print(self.condition_stack[0])


console = Processor()
console.prompt = " > "
console.cmdloop("Welcome to Machine!")

