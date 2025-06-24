echo "STOP THE NODE **************************"
read -p "Press enter to continue"

rm -rf /home/fernando/mempool_node_analysis/database
cp -R /home/fernando/mempool_node_analysis/database-1278138 /home/fernando/mempool_node_analysis/database

echo "RUN THE NODE AND DISABLE THE BREAKPOINTS **************************"
read -p "Press enter to continue"

python send_raw_blocks_1.py
read -p "Press enter to continue"

python send_raw_tx.py
read -p "Press enter to continue"

python send_raw_blocks_2.py

read -p "Press enter to continue"

python send_raw_blocks_3.py


echo "ENABLE BREAKPOINTS **************************"
read -p "Press enter to continue"

python send_raw_blocks_4.py
