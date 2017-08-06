from cmd import Cmd
from functools import wraps

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
        super(NotInWhileError, self).__init__(self, "Not in a while loop!", None)


class NotInIfError(Exception):

    def __init__(self):
        super(NotInIfError, self).__init__(self, "Not in if branch!", None)


class Processor(Cmd):

    memsize = 16
    memset = {}
    regset = {"R1": "Null", "R2": "Null", "R3": "Null", "R4": "Null", "AC": "Null", "SP": 0}
    program_stack = []
    condition_stack = []
    priviledged_reg = ["AC", "SP", "IR"]
    defined_conds = ["lt", "gt", "lte", "gte", "eq", "neq"]


    def postcmd(self, stop, line):
        # print("SP : %d" % self.regset["SP"])
        if self.regset["SP"] != len(self.program_stack):
            args = str(self.program_stack[self.regset["SP"]][1]).replace("[", "").replace("]", "").replace("'", "")
            self.cmdqueue.append(self.program_stack[self.regset["SP"]][0].__name__.replace("do_", "", 1)+" "+args)
        return Cmd.postcmd(self, stop, line)


    def push_ps(self, args):
        if self.regset["SP"] == len(self.program_stack):
            self.program_stack.append(args)
            # print("Pushing to stack %s" % str(args))
        self.regset["SP"] += 1


    def default(self, line):
        print("[ERROR] Command not recognised! See `help`.")
        return 0

    
    def cexec(self):
        if len(self.condition_stack) == 0:
            return True
        else:
            return self.evalcond(self.condition_stack[0][1], self.condition_stack[0][2], self.condition_stack[0][3])


    def do_help(self, args):
        """
Shows help on the command usage.
Syntax :
        help [command]
[command] can be any of the available commands
        """
        self.push_ps([self.do_help, args])
        return Cmd.do_help(self, args)


    def noargs(self, req, name):
        nostring = "One"
        iss = ""
        verb = "is"
        if req > 1:
            iss = "s"
            verb = "are"
        if req == 2:
            nostring = "Two"
        elif req == 3:
            nostring = "Three"
        print("[ERROR] %s argument%s %s required for `%s`! See `help %s`." % (nostring, iss, verb, name, name))

    
    def argcheck(num):
        def narg(func):
            name = func.__name__.replace("do_", "", 1)
            @wraps(func)
            def checkargs(self, args):
                if len(args) == 0:
                    return self.noargs(num, name)
                else:
                    arglist = args.split(", ")
                    if len(arglist) != num:
                        def wrongsyntax(self, args):
                            print("[ERROR] Wrong syntax for function `%s`! See `help %s`." % (name, name))
                        return wrongsyntax(self, args)
                    else:
                        if num == 1:
                            return func(self, arglist[0])
                        else:
                            return func(self, arglist)
            return checkargs
        return narg
     

    @argcheck(1)
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
                if self.cexec():
                    self.perform_print(args)
                self.push_ps([self.do_print, args])
            except NotInMemoryError:
                print("[ERROR] No such variable in memory")
    
    
    @argcheck(2)
    def do_load(self, args):
        """
Loads a value in register.
Syntax :
        load [source], [destination]
[source] can be either a variable name or a register number
[destination] should must be a register number
        """
        if args[1] in self.priviledged_reg:
            print("[ERROR] Unable to access priviledged registers!")
        else:
            try:
                if self.cexec():
                    self.perform_load(args[0], args[1])
                self.push_ps([self.do_load, args])
            except IllegalRegisterError:
                print("[ERROR] Bad register number %s " % args[1])
            except NotInMemoryError:
                print("[ERROR] No such variable in memory")


    @argcheck(2)
    def do_store(self, args):
        """
Stores the value from a register to memory
Syntax :
        store [source], [destination]
[source] must be a register
[destination] can be either a variable name or register
If no such variable exists in memory, a new one will be created implicitly
        """
        try:
            if self.cexec():
                self.perform_store(args[0], args[1])
            self.push_ps([self.do_store, args])
        except IllegalRegisterError:
            print("[ERROR] Bad register number %s!" % args[0])
        except ValueError:
            print("[ERROR] Bad variable name %s!" % args[1])
        except MemoryFullError:
            print("[ERROR] Memory full!")
                
    
    @argcheck(2)        
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
        try:
            self.perform_let(args[0], args[1])
            self.push_ps([self.do_let, args])
        except MemoryFullError:
            print("[ERROR] Memory full")
        except ValueError:
            print("[ERROR] Bad variable name %s " % args[1])


    @argcheck(1)
    def do_unlet(self, args):
        """
Unloads a variable from memory.
Syntax :
        unlet [variable]
[variable] must be a variable in memory
Error will be raised if no such variable exists in memory.
        """
        try:
            self.perform_unlet(args)
            self.push_ps([self.do_unlet, args])
        except NotInMemoryError:
            print("[ERROR] No such variable exists in memory!")

    
    @argcheck(1)
    def do_incr(self, args):
        """
Increments the value of a variable by unity.
Syntax :
        incr [destination]
[destination] can be either a variable of a register address
        """
        try:
            if self.cexec():
                self.perform_incr(args)
            self.push_ps([self.do_incr, args])
        except ValueError:
            print("[ERROR] Value stored in %s is not a number!" % args)
        except NotInMemoryError:
            print("[ERROR] No such variable exists in memory!")


    @argcheck(1)
    def do_decr(self, args):
        """
Decrements the value of a variable by unity.
Syntax :
        incr [destination]
[destination] can be either a variable of a register address
        """
        try:
            if self.cexec():
                self.perform_decr(args)
            self.push_ps([self.do_decr, args])
        except ValueError:
            print("[ERROR] Value stored in %s is not a number!" % args)
        except NotInMemoryError:
            print("[ERROR] No such variable exists in memory!")
    
    
    @argcheck(3) 
    def do_while(self, args):
        """
Execute a block of statements repeatedly based on a condition. This call basically sets a label on the next instruction to be later executed.
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
        try:
            self.perform_while(args[0], args[1], args[2])
        except ValueError:
            print("[ERROR] Both arguments must be numeric!")
        except UndefinedConditionError:
            print("[ERROR] Condition is not defined!")
        except NotInMemoryError:
            print("[ERROR] Variable is not in memory!")


    def do_endwhile(self, args):
        """
Denotes the termination and starts a while loop. This call basically invokes one of the conditional jump calls on the last while label created.
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

    
    @argcheck(1)
    def do_setlabel(self, args):
        """
Assigns the next instruction with a label, to be later used with various jump calls. This call is not pushed to the call stack itself.
Syntax :
        setlabel [label]
[label] must be a valid variable name
If no such label exists, a new label will be created in memory.
However, if such a label exists, its value will be replaced.
        """
        try:
            self.perform_setlabel(args)
        except ValueError:
            print("[ERROR] Label must be a valid variable name!")
        except MemoryFullError:
            print("[ERROR] Memory is full!")


    @argcheck(1)
    def do_jmp(self, args):
        """
Jumps to the argument label in the call stack, and executes subsequent calls until it reaches the end of the stack.
This is however, an one time call. This call itself will not be saved in program stack. Hence to perform conditional
jump, use other calls. See `help` for more information.
Syntax :
        jmp [label]
[label] must be a predefined label name.
If no such label exists, and error will be raised.
        """
        try:
            self.perform_jmp(args)
        except NotInMemoryError:
            print("[ERROR] No such label exists in memory!")


    @argcheck(3)
    def do_jneq(self, args):
        """
Jump to the specific label if arg1 is not equal to arg2
Syntax :
        jneq [label], [arg1], [arg2]
[label] must be a predefined label
[arg1] must be a variable name or register name
[arg2] can be any constant, variable name, or register name
        """
        try:
            self.perform_cjmp("neq", self.do_jneq, args[0], args[1], args[2])
        except NotInMemoryError:
            print("[ERROR] The label or variables not in memory!")
        except ValueError:
            print("[ERROR] All variables must contain numeric value")


    @argcheck(3)
    def do_jeq(self, args):
        """
Jump to the specific label if arg1 is equal to arg2
Syntax :
        jne [label], [arg1], [arg2]
[label] must be a predefined label
[arg1] must be a variable name or register name
[arg2] can be any constant, variable name, or register name
        """
        try:
            self.perform_cjmp("eq", self.do_jeq, args[0], args[1], args[2])
        except NotInMemoryError:
            print("[ERROR] The label or variables not in memory!")
        except ValueError:
            print("[ERROR] All variables must contain numeric value")


    @argcheck(3)
    def do_jgt(self, args):
        """
Jump to the specific label if arg1 is greater than arg2
Syntax :
        jne [label], [arg1], [arg2]
[label] must be a predefined label
[arg1] must be a variable name or register name
[arg2] can be any constant, variable name, or register name
        """
        try:
            self.perform_cjmp("gt", self.do_jgt, args[0], args[1], args[2])
        except NotInMemoryError:
            print("[ERROR] The label or variables not in memory!")
        except ValueError:
            print("[ERROR] All variables must contain numeric value")


    @argcheck(3)
    def do_jlt(self, args):
        """
Jump to the specific label if arg1 is less than arg2
Syntax :
        jne [label], [arg1], [arg2]
[label] must be a predefined label
[arg1] must be a variable name or register name
[arg2] can be any constant, variable name, or register name
        """
        try:
            self.perform_cjmp("lt", self.do_jlt, args[0], args[1], args[2])
        except NotInMemoryError:
            print("[ERROR] The label or variables not in memory!")
        except ValueError:
            print("[ERROR] All variables must contain numeric value")


    @argcheck(3)
    def do_jgte(self, args):
        """
Jump to the specific label if arg1 is greater than or equal to arg2
Syntax :
        jne [label], [arg1], [arg2]
[label] must be a predefined label
[arg1] must be a variable name or register name
[arg2] can be any constant, variable name, or register name
        """
        try:
            self.perform_cjmp("gte", self.do_jgte, args[0], args[1], args[2])
        except NotInMemoryError:
            print("[ERROR] The label or variables not in memory!")
        except ValueError:
            print("[ERROR] All variables must contain numeric value")


    @argcheck(3)
    def do_jlte(self, args):
        """
Jump to the specific label if arg1 is less than or equal to arg2
Syntax :
        jne [label], [arg1], [arg2]
[label] must be a predefined label
[arg1] must be a variable name or register name
[arg2] can be any constant, variable name, or register name
        """
        try:
            self.perform_cjmp("lte", self.do_jlte, args[0], args[1], args[2])
        except NotInMemoryError:
            print("[ERROR] The label or variables not in memory!")
        except ValueError:
            print("[ERROR] All variables must contain numeric value")



    @argcheck(3)
    def do_if(self, args):
        """
Performs a conditional execution of the subsequent statements until the next endif or else
Syntax :
        if [cond], [arg1], [arg2]
[cond] will be one of
        1. lt : less than
        2. gt : greater than
        3. lte : less than or equal to
        4. gte : greater than or equal to
        5. eq : equal to
        6. ne : not equal to
[arg1] can be either of a memory variable or a register number
[arg2] can be either of a constant, memory variable or register number
If the given condition is false, no statements will be executed until the next
else or endif is encountered.
        """
        try:
            self.perform_if(args[0], args[1], args[2])
            self.push_ps([self.do_if, args])
        except ValueError:
            print("[ERROR] All arguments must be numeric")
        except UndefinedConditionError:
            print("[ERROR] Given condition is not defined")

    
    def do_endif(self, args):
        """
Terminates an if condition.
Syntax :
        endif
This call must be issued after an if call, otherwise an error will be raised.
This function doesn't take any arguments.
        """
        if len(args) != 0:
            print("[ERROR] This function does not take any arguments!")
        else:
            try:
                self.perform_endif()
            except NotInIfError:
                print("[ERROR] Not in an if block!")


    def do_quit(self, args):
        """
Quits the system.
After calling this method, present instance of the system is freed,
along with all of its registers, memory, and stacks.
Syntax :
        quit
This function does not take any arguments.
        """
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

    def check_jargs(self, cond, source, dest):
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

    def perform_while(self, cond, source, dest):
        self.check_jargs(cond, source, dest)
        label = "while_"+str(len(self.condition_stack))
        self.condition_stack.append([label, cond, source, dest]) 
        self.perform_setlabel(label)


    def getval(self, arg):
        if arg in self.regset:
            return float(self.regset[arg])
        elif arg in self.memset:
            return float(self.memset[arg])
        else:
            return float(arg)


    def evalcond(self, cond, source, dest):
        getval = self.getval
        if (cond == "eq" and getval(source) == getval(dest)) \
        or (cond == "neq" and getval(source) != getval(dest)) \
        or (cond == "lt" and getval(source) < getval(dest)) \
        or (cond == "gt" and getval(source) > getval(dest)) \
        or (cond == "lte" and getval(source) <= getval(dest)) \
        or (cond == "gte" and getval(source) >= getval(dest)):
            return True
        else:
            return False

    def perform_endwhile(self, args):
        if len(self.condition_stack) == 0:
            raise NotInWhileError
        else:
            # print("[DEBUG] Condition stack "+str(self.condition_stack))
            arg = self.condition_stack.pop()
            label = arg[0]
            cond = arg[1]
            source = arg[2]
            dest = arg[3]
            func = getattr(self, "do_j"+cond)
            self.perform_cjmp(cond, func, label, source, dest)


    def perform_setlabel(self, args):
        # print("[DEBUG] Setting label %s to pointer %d" % (args, self.regset["SP"]))
        self.perform_let(self.regset["SP"], "jmplabel_"+args)


    def perform_jmp(self, args):
        # print("[DEBUG] Jumping to label %s " % args)
        # print("[DEBUG] Memory layout : "+str(self.memset))
        self.perform_load("jmplabel_"+args, "SP")


    def perform_cjmp(self, cond, func, label, source, dest):
        self.check_jargs(cond, source, dest)
        self.push_ps([func, [label, source, dest]])
        if self.cexec() and self.evalcond(cond, source, dest):
            self.perform_jmp(label)
        else:
            if label == "while_0":
                i = 0
                while 1:
                    try:
                        self.perform_unlet("jmplabel_while_"+str(i))
                        i += 1
                    except NotInMemoryError:
                        break
        

    def perform_if(self, cond, source, dest):
        self.check_jargs(cond, source, dest)
        label = "iflabel_"+str(len(self.condition_stack))
        self.condition_stack.append([label, cond, source, dest])

    
    def perform_endif(self):
        exp = "iflabel_"+str(len(self.condition_stack)-1)
        if len(self.condition_stack) > 0 and exp == self.condition_stack[-1][0]:
            self.condition_stack.pop()
        else:
            raise NotInIfError


console = Processor()
console.prompt = " > "
console.cmdloop("Welcome to Machine!")

