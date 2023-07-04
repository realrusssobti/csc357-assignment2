make && wget -O fs_tests.tgz -q https://github.com/amigler/csc357-s23/blob/main/a2/fs_tests.tgz?raw=true && tar -xf fs_tests.tgz
./fs_simulator fs1 < input.sh > actual_out.txt && diff expected_out.txt actual_out.txt
/bin/ls -ls fs1 | awk '{print $6,$10}' > fs1_files.txt && /bin/ls -ls fs2 | awk '{print $6,$10}' > fs2_files.txt
diff fs1_files.txt fs2_files.txt