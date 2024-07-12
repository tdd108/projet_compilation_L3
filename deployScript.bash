#!/bin/bash
good=0
semerr=0
synerr=0
warning=0
for file in ./test/good/*
do 
    echo "$file"
    ./bin/tpcc < "$file"
    result=$?

    if [ $result -eq 0 ]
    then 
        let good=good+1
    fi
done

for file in ./test/sem-err/*
do 
    echo "$file"
    ./bin/tpcc < "$file"
    result=$?

    if [ $result -eq 2 ]
    then 
        let semerr=semerr+1
    fi
done

for file in ./test/syn-err/*
do 
    echo "$file"
    ./bin/tpcc < "$file"
    result=$?

    if [ $result -eq 1 ]
    then 
        let synerr=synerr+1
    fi
done

for file in ./test/warn/*
do 
    echo "$file"
    ./bin/tpcc < "$file"
    result=$?

    let warning=warning+1
done


echo "Total good tests: $good"
echo "Total semerr tests: $semerr"
echo "Total synerr tests: $synerr"
echo "Total warning tests: $warning"