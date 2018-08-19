#!/bin/bash

function dirfile(){
    for i in `ls $2`
        do
            if [ -d $2"/"$file ]
            then
                getdir $dir_or_
            else
                ./$1 i >> ./out
        done

}

dirfile $1 $2