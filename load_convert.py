#%%
# # Convert the files to log files
# The source for this part is: https://github.com/chengtx/WorldCup98

import os
import fnmatch

def main():

    print("Start to unzip file")

    pattern = '*.gz'
    root_path = './input'
    tool_path = './ita_public_tools/'

    list_dirs = os.walk(root_path)
    for root, dirs, files in list_dirs:
        for f in fnmatch.filter(files, pattern):
            print(f)
            cmd = 'gzip -dc input/' + f + ' | ' + tool_path + 'bin/recreate ' + tool_path + 'state/object_mappings.sort > output/' + f[:-3] + '.log'
            os.system(cmd)

if __name__ == "__main__":
    #main()
    pass

#%%
# # Read the files and parse them
def get_pattern_files(root_path, pattern):
    all_files = []

    list_dirs = os.walk(root_path)
    for root, dirs, files in list_dirs:
        for f in fnmatch.filter(files, pattern):
            all_files.append(os.path.join(root,f))
    
    return all_files

pattern = '*.log'
root_path = './output'

all_files = get_pattern_files(root_path, pattern)

#%%
# ## Parse the data
import re
from datetime import datetime

# Convert the datetime objects to pandas tables and get the number of requests
import pandas as pd

def parse_file(input_file, output_file):
    with open(input_file, 'r', encoding='ISO-8859-1') as file_handle:
        # print(input_file, ' --> ', output_file)
        time_objects = []
        for line in file_handle:
            time_str = re.search("\[.*\]", line)
            time_str = time_str.group()[1:-1]
            time_str = time_str.split(' ')[0]
            # format: 30/Apr/1998:21:30:17
            datetime_object = datetime.strptime(time_str, '%d/%b/%Y:%H:%M:%S')
            time_objects.append(datetime_object)

        dates = pd.to_datetime(time_objects)

        df = pd.DataFrame(data = {'dummy': 1}, index=dates)
        df['per'] = dates.to_period('T')
        df_count = df.groupby('per')['dummy'].sum()

        df_count.to_csv(output_file, header=False)

        return time_objects
        
        
#%%

from tqdm.autonotebook import tqdm
tqdm.pandas()

for input_file in tqdm(all_files):
    try:
        output_file = input_file.replace('output', 'results').replace('.log','.csv')
        time_objects = parse_file(input_file, output_file)
    except Exception as e:
        print('Exception caused failure:', input_file, str(e))

#%%
# ## Merge CSV Files
import pandas.io.common

root_path = './results'
all_csv_files = get_pattern_files(root_path, '*.csv')

df = None
for csv_file in all_csv_files:
    try:
        new_df = pd.read_csv(csv_file, header=None, 
                    names=['period','count'], index_col=0, parse_dates=True)
        if df is None:
            df = new_df
        else:
            df = df.append(new_df)
    except pandas.io.common.EmptyDataError:
        print('file', csv_file, 'is empty!')

df = df.groupby('period').sum()
df.to_csv('invocation_count.csv')

df.plot()