mkdir bin && echo "Created bin/ directory." || echo "Directory bin/ already exists."
mkdir build && echo "Created build/ directory." || echo "Directory build/ already exists."
cd build
mkdir cmake_generated && echo "Created build/cmake_generated/ sub-directory." || echo "Sub-directory build/cmake_generated/ already exists."
cmake ../cmake/
make -j `nproc` && echo "Build completed." || echo "Build errors!  Check output log."
cd ..
mkdir headers/standard_masala_plugins && echo "Created headers/standard_masala_plugins/ directory." || echo "Directory headers/standard_masala_plugins/ already exists."
rm -r headers/standard_masala_plugins/*
python3 ./code_templates/copy_headers.py standard_masala_plugins selectors_api src headers/standard_masala_plugins
python3 ./code_templates/copy_headers.py standard_masala_plugins optimizers_api src headers/standard_masala_plugins
python3 ./code_templates/copy_headers.py standard_masala_plugins registration_api src headers/standard_masala_plugins
