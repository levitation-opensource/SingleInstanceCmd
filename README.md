## SingleInstanceCmd
Tool for ensuring that a process specified in the arguments is launched only one at a time. Similar to flock tool in Linux.

By default, also a job object is created, which ensures that when SingleInstanceCmd.exe is killed then the child processes launched from it are also killed.

### Usage:
SingleInstanceCmd.exe mutexname [-] cmd [args]
<br>"-" indicates that no job object must be created (child processes will live even when SingleInstanceCmd.exe is killed)
