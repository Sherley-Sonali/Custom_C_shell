# Assumptions 

## Spec - 3
* Assuming /home/username/path/~/folder as an unacceptable path
* If  there is no previous directory :  warning : 
*     "OLDPWD not set" (ref : Spec-3 Q14)

## Spec - 4
* No multiple paths for reveal : error message : 
*     "Error - scandir: No such file or directory"

## Spec - 5
* Even erraneous commands are being stored in the history even the ones with str instances of "log"
* The commands executed using "log execute x" are not stored in history
* Hidden file to store log : ~/.command_log

## Spec - 6
* The time taken by fg processes in reflected in all prompts post its execution
* The times are separated by a ";"
*     <sherleysonalip@intel-corei5:~>  sleep 3
*     <sherleysonalip@intel-corei5:~ sleep : 3s>  log execute 1
*     <sherleysonalip@intel-corei5:~ sleep : 3s; sleep : 3s; log : 3s>  
* Maximum that can be reflected in the prompt : 100
* The reflected time is absolute time taken for process execution
* Erraneous commands and the commands implemented (hop etc.) would start a bg but not run : error : and then exit normally
*     error: No such file or directory
* The command "exit" is dealt the following ways in fg :
*     <sherleysonalip@intel-corei5:~>  exit
*     Exiting shell... 
* in bg :
*   <pre>exit &
    [1] 220234
    Exiting shell...
    Exit exited normally (220234)
</pre>
* Separate warning when any process abruptly stopped (when interactive processes like vi are run):
*     [14]+  Stopped                 ./a.out  

## Spec - 7
* Units of storage of virtual memory is : number of pages 
* When unable to render the executable path of a process : error :
*     Error reading executable path

## Spec - 8
* If only a single file is found (with the -e flag) and it is an executable file , then the file will get executed and if it is non-executable then its contents are printed

# Formatting

## Spec - 3 :
 ### Acceptable paths 
 
 * absolute paths from root '/'
 * relative paths and paths from home directory of the form ~/...
 * Also supports hop . .. - ~
 
 ### Maximum path length
* All executable paths and other paths are greater than 512 B
* If a path is concatenation of two paths, path lengths = 1024 B and total is 4096 B
* Executable path length in PROCESS details : 256 B

## Spec - 4
* When "reveal path/to/file/not/directory" is given , the file name is printed, with -l flag its details are printed

## Spec - 5
* Commands <pre>"cmd \<arg>" , "cmd \<arg>   " , "cmd    \<arg>" are all treated as different commands in log</pre>

