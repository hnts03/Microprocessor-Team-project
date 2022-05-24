# [Microprocessor-Team-project]

## This project is for making FSM structure of electronic washer
---  
### Using Board : X-Hyper320TKU Board
### This Machine works with **4 states**  
[1. Initial state] -> [2. Time insertion state] -> [3. Washing state] -> [4. Washing Done state]   

---

1. Initial state(init state)  
    1. program starts with type "./washwer" in CMD window  
        ```cmd
        ./washer
        ```

    2. you can use kind of arguments
        ```cmd
        ./washer led-on dot-on fnd-on
        ```
        each of arguments operates specific working of each parts

    3. If program started, you could see menu of selecting mode for machine.  
    Insert key with keyboard and press enter.  
    Then current state will change to Time insertion state.

2. Time insertion state(TI state)  
   Insert Time of machine working.

    1. Insert number with keypad (0~9) buttons.  
        This system uses Decimal numbers.  

    2. Erase the 1 recent number with keypad 'e' button. 
    3. Final Time precision with keypad 'f' button.  
        If 'f' button pressed, current state will change to Washing state.

3. Washing state(WS state)  
    Machine work as much as the time entered in the previous state.  

    After the time that you enter, current state will change to Washing Done state.  

4. Washing Done state(WD state)  
   In this state, press 'b' button of keypad, program will exit.


     
