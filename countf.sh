#!/bin/bash
#countf
#recursively counts the number of files or directories in a given directory
#if no input, counts the number of files or directories in the current directory
#if directories specified, count the number of files/directories in the given directory
#if -help is inputted, instructions for script will show.
#@author Juan Valenzuela
#@version 2.4.17

total=0
fileCount=0
directCount=0

function findDirect
{
  local directory
  for file in *; do
    if [ -d $file ]; then
      if [ "$file" == "$1" ]; then
        cd $file
        directory="$(pwd)" #saves file path to directory
        cd .. #goes back to parent directory
        echo $directory #sends file path to what called this function
        return
      else
        cd $file
        directory=$(findDirect $1) #begin searching through encountered directory
        cd ..
      fi
      if [ "$directory" != "" ]; then #check to see if the directory path was found
        echo $directory
        return
      fi
    fi
  done
}

function count
{
  #This block sets the directory if one is specified.
  if [ "$1" != "" ]; then
    local currDirect=$(pwd)
    cd
    local directLoc="$(findDirect $1)"
    if [ "$directLoc" != "" ]; then
      cd $directLoc
      #pwd
    else
      printf "\ndirectory $1 does not exist.\nPlease check spelling and capitalization.\n\n"
      cd $currDirect
      #pwd
      return
    fi
  fi

  #Begin counting files
  for file in *; do
    if [ -f $file ]; then
      #printf "found a file @ $(pwd)\n"
      ((total++))
      ((fileCount++))
    elif [ -d $file ]; then
      #printf "found a directory @ $(pwd)\n"
      ((total++))
      ((directCount++))
      count $file
      cd .. #go back to previous directory
      continue
    fi
  done
}

function help
{
  printf "This program counts the number of files and directories in a given directory.\n"
  printf "\nIf no argument is given, the program will count the files from the the current directory.\n"
  printf "\nIf a directory is specified, the program will start counting from that directory.\n"
  printf "\nTo input a directory, call the program followed by '-d DirectoryName'.\n"
}

printf '\nType -h as an argument for help and list of commands.\nExample: ./countf.sh -h\n\n'
if [ "$1" == "-h" ];  then
  help
  exit
elif [ "$1" != "" ]; then
  while [ "$1" != "" ]; do #iterate through arguments
    count $@
    shift
  done
else
  count
fi
#pwd
printf "\nThe total number of items is  $total\nThe number of files is $fileCount\nThe number of directories is $directCount\n"
exit 0
