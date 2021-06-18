# pip uninstall -y helios 
# pip install -e ./
python setup.py build_ext --inplace install
echo "\nTesting in python..."
python docs/tutorial/example.py
