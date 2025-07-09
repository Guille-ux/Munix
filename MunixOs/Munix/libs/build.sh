rm -rf build/*
cat ~/libcs2_build.json > pybuild.json
python3 pybuild.py
rm -rf pybuild.json
ar rcs libcs2.a build/*
