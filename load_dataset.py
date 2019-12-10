#%% [markdown]
# # Load the Dataset and grab a subset
# In this python code we grab the dataset and scale a subset of dataset to use as
# sample workload in our application.
#%%
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# csv_file = 'invocation_count.csv'
csv_file = 'https://raw.githubusercontent.com/nimamahmoudi/faas-keep-alive-modeling/master/datasets/wc98/invocation_count.csv'
df = pd.read_csv(csv_file, index_col=0, parse_dates=True)
df = df.groupby('period').sum()
# similar to paper: Stochastic Resource Provisioning for
#                   Containerized Multi-Tier Web Services in Clouds
sub_df = df['1998-06-30 08:00:01':'1998-07-01 08:00:00']

sns.set()

plt.figure(figsize=[10,8])
# sub_df.plot()
# plt.savefig('test.png', dpi=300)

# Scaling the number of requests to another maximum
scaled_max = 100
scaled_sub_df = (sub_df / sub_df['count'].max() * scaled_max).apply(lambda x: round(x))
scaled_sub_df['count'] = scaled_sub_df['count'].apply(lambda x: int(x))

scaled_sub_df.plot()

scaled_sub_df['count'].values

