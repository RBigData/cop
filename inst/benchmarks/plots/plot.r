library(ggplot2)
library(ggthemes)
library(reshape2)

df = read.csv("runs.csv", stringsAsFactors=FALSE)
df$procs = df$size*df$threads

df_sub = df[, !(names(df) %in% c("m", "n", "size", "threads", "t_dbl_min", "t_dbl_mean", "t_flt_min", "t_flt_mean"))]
df_melt = melt(df_sub, c("method", "procs"))
precision = gsub(df_melt$variable, pattern="t_dbl_max", replace="fp64")
precision = gsub(precision, pattern="t_flt_max", replace="fp32")
df_melt$variable = precision

g = ggplot(df_melt, aes(procs, value, color=method)) +
  theme_minimal() +
  geom_point() +
  geom_line() + 
  scale_color_tableau() + 
  facet_wrap(~variable, nrow=1) +
  xlab("Cores") + 
  ylab("Run Time in Seconds (lower is better)") + 
  labs(color="Method") + 
  theme(legend.position="bottom") +
  ggtitle("QR Benchmark on 10,000x2500 matrix (200 MB)")

g
# ggsave(g, file="pagerank.png")
