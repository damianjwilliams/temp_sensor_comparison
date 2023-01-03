library(rjson)
library(tidyverse)
library(purrr)
library(tools)
library(jsonlite)
library(gghighlight)
library(splitstackshape)


FIJI_folder_name <-"/Users/damianwilliams/Dropbox/Programming/Python/All_temp_sensors/data/json_sensor_reformat/"
#FIJI_folder_name <- "C:/Users/Damian/Desktop/test_sensor"


setwd(FIJI_folder_name)

file_list <-
  list.files(recursive = TRUE,
             include.dirs = F,
             pattern = "\\.json$")



f_convert <-function(x){data.frame(lapply(x, "length<-", max(lengths(x))))%>%
  mutate(replicate= row_number())%>%
  pivot_longer(-replicate)}


f_convert_2 <-function(x){data.frame(lapply(x, "length<-", max(lengths(x))))%>%
    mutate(replicate= row_number())}

#f_convert_2(result)  


#########3




######
file_list

alljsons <- lapply(setNames(nm = file_list), jsonlite::read_json)


ms_to_date = function(ms, t0="1970-01-01", timezone="EST") {
 sec = as.numeric(ms / 1000)
  as.POSIXct(sec, origin=t0, tz=timezone)
}
ms_to_date(1667062775010)


datalist = list()




for (idx in file_list) {
  #print(idx)
  result <- rjson::fromJSON(file = idx)%>%
  f_convert_2
  #print(result)
  time_ms <- as.numeric(file_path_sans_ext(idx))
  print(time_ms)
  print(file_path_sans_ext(idx))
  result$rec_time <- ms_to_date(time_ms)  # maybe you want to keep track of which iteration produced it?
  datalist[[idx]] <- result # add it to your list
 
}

big_data = do.call(bind_rows, datalist)

plot_me <-big_data %>%
  pivot_longer(-c(rec_time,replicate))%>%
  filter(between(value,21.5,30))%>%
  mutate(replicate = as_factor(replicate))

ggplot(plot_me,aes(x=rec_time,y=value,color=interaction(name,replicate)))+
  geom_line(show.legend = FALSE,size=0.2)+
  labs(x="Time",y="Temperature (C)")
  ggsave("/Users/damianwilliams/Desktop/all_data_over_time.png",height = 4,width = 6)
  
  




#Plot_individual points

single_point_data <- plot_me %>%
  pivot_wider(names_from = c(name,replicate),values_from = value,names_sep = "xx")%>%
  filter(row_number()>4589)%>%
  pivot_longer(-rec_time)%>%
  drop_na()%>%
  cSplit("name",sep = "xx")%>%
  rename(sensor = name_1)%>%
  rename(replicate = name_2)
 
sensor_names<-single_point_data %>%
distinct(sensor,replicate)

sensor_names_updated <- sensor_names %>%
  mutate(new_name = ifelse(grepl("DHT.22",sensor,fixed=TRUE),"DHT22",sensor))

#rename ESP8266 sensors
sensor_list_redone_ESP8266<-sensor_names_updated %>%
  filter(grepl("_",sensor,fixed=TRUE))%>%
  mutate(updated_name = gsub("_esp8266","",sensor,fixed = TRUE))%>%
  mutate(updated_name = gsub("DHT","DHT11",updated_name,fixed = TRUE))%>%
  mutate(board = "ESP8266")%>%
  select(-c(new_name))%>%
rename(match_name = sensor)%>%
  rename(sensor = updated_name)

  #rename ESP32
sensor_list_redone_ESP32<-sensor_names_updated%>%
  filter(!(grepl("_esp8266",sensor,fixed=TRUE)))%>%
  mutate(board = "ESP32")%>%
  rename(match_name = sensor)%>%
  rename(sensor = new_name)

new_sensor_names <- rbind(sensor_list_redone_ESP32,sensor_list_redone_ESP8266)

single_point_data_rn <- single_point_data %>%
  rename(match_name = sensor)


single_point_data_new <- right_join(new_sensor_names,single_point_data_rn,by = c("match_name","replicate"))%>%
  mutate(sensor = gsub("LM45A","LM75A",sensor))%>%
  mutate(sensor = gsub("MCP","MCP9808",sensor))%>%
  mutate(sensor = gsub("Thermocouple","Type K thermocouple",sensor))%>%
  mutate(sensor = gsub("RTD","PT1000 RTD",sensor))%>%
  mutate(sensor = gsub("TMP35","TMP36",sensor))
  
  

unique(single_point_data_new$sensor)

average_plot_data <- single_point_data_new %>%
  drop_na()%>%
  group_by(sensor,replicate,board)%>%
  summarise(average_reading = mean(value))

reference_temp <- average_plot_data %>%
  filter(sensor == "Eutronics")%>%
  ungroup()%>%
  select(4)%>%
  as.numeric()

ref_temp_line <- single_point_data_new %>%
  filter(sensor == "Eutronics")%>%
  select(-sensor)


ggplot(single_point_data_new,aes(x=rec_time,y=value,group=interaction(board,replicate)))+
  geom_line()+
  facet_wrap(~sensor)+
  geom_line(data = ref_temp_line, aes(x=rec_time,y = value),color="red",alpha=0.3)+
  theme(axis.text.x = element_text(angle = 45,hjust = 1))+
  labs(y="Average temperature (C)",x="Time")
ggsave("/Users/damianwilliams/Desktop/7_min_portion.png",height = 5,width = 7)


# ggplot(single_point_data,aes(x=rec_time,y=value,group=interaction(sensor,replicate)))+
#   geom_line(aes(color=sensor))+
#   #facet_wrap(~sensor)+
#   #geom_line(data = ref_temp_line, aes(x=rec_time,y = value),color="red",alpha=0.3)+
#   theme(axis.text.x = element_text(angle = 45,hjust = 1))+
#   labs(y="Average temperature (C)",x="Time")
# 

  
  

#groups

f_group <- function(x) {
  dplyr::case_when(
    grepl("^TMP", x) ~ "A",
    grepl("DHT|BME", x) ~ "B",
    grepl("hermo|RTD", x) ~ "C",
    grepl("^LM|MCP", x) ~ "D",
    grepl("ADT|AHT|DS18", x) ~ "E",
    
    TRUE ~ "Unknown"
  )
}

average_plot_data <- average_plot_data %>%
  mutate(sensor_group = f_group(sensor))%>%
  filter(!(grepl("Eutronics",sensor)))


ggplot(average_plot_data,aes(x=sensor,y=average_reading))+
  geom_point()+
  geom_hline(yintercept=reference_temp, linetype="dashed", color = "red")+
  facet_wrap(~sensor_group,scales = "free_x")+
  theme(strip.background = element_blank(),
        strip.text.x = element_blank())+
  labs(y="Average temperature",x="Sensor")
ggsave("/Users/damianwilliams/Desktop/average.png",height = 5,width = 7)


#group=interaction(sensor, replicate)

###Create frames



 





sensor_list <- unique(plot_me$name)

for (idx in sensor_list) {
  
  plot_a <- ggplot(plot_me,aes(x=rec_time,y=value,group=interaction(replicate,name)))+
    geom_line(aes(color=name))+
    gghighlight(grepl(idx,name))
  print(idx)
  save_name <- paste(idx,".png",sep="")
  print(save_name)
  ggsave(filename=save_name,plot = )
 
}

#ggplot(plot_me,aes(x=rec_time,y=value,group=interaction(replicate,name)))+
 # geom_line(aes(color=name))+
 # gghighlight(grepl("TMP35|Eut",name))


####test slice####

FIJI_folder_name <- "/Users/damianwilliams/Desktop/r_output"
setwd(FIJI_folder_name)

plot_me_slice <-big_data %>%
  pivot_longer(-c(rec_time,replicate))%>%
  mutate(name=gsub("_",".",name,fixed=TRUE))

data_for_subsetting <- plot_me_slice %>%
  pivot_wider(names_from = c(name,replicate),values_from=value)


#number_of_plots = (nrow(test_scroll)-102)




# for (x in 1:number_of_plots) {
#   
#   current_plot_df <- data_for_subsetting[(1+x):(length_of_plots+x),]
#   
#   current_plot_df_plot <- current_plot_df%>%
#     pivot_longer(-rec_time, names_to = c("Sensor", "Replicate"),
#                  names_sep = "_",values_to = "Reading")
#   
#   ggplot(current_plot_df_plot,
#          aes(x = rec_time,
#              y = Reading,
#              group=interaction(Sensor, Replicate)))+
#     geom_line(aes(color=Sensor))#+
#     #gghighlight(Sensor == Eutronics)+
#   
#     
#   coord_cartesian(ylim = c(22,28))+
#     theme_classic()+
#     theme(legend.position = "none")
#   ggsave(
#     file = paste("Basal_current_addition_quant_hippo_",x,".jpg",sep = ""),
#     bg = "transparent",
#     width = 6,
#     height = 6)
#   
#   #progress = paste("progress: ",x,sep="")
#   print(x)
#   }


####Doulbe loop##############

####test slice####

plot_me_slice <-big_data %>%
  pivot_longer(-c(rec_time,replicate))%>%
  mutate(name=gsub("_",".",name,fixed=TRUE))%>%
  filter(between(value,19,26))


#Rename replicates

sensor_list_redone <- unique(plot_me_slice[c("replicate","name")])

sensor_list_redone <- sensor_list_redone %>%
  mutate(new_name = ifelse(grepl("DHT.22",name,fixed=TRUE),"DHT22",name))

#rename ESP8266 sensors
sensor_list_redone_ESP8266<-sensor_list_redone %>%
  filter(grepl(".",new_name,fixed=TRUE))%>%
  mutate(updated_name = gsub(".esp8266","",name,fixed = TRUE))%>%
  mutate(updated_name = gsub("DHT","DHT11",updated_name,fixed = TRUE))%>%
  mutate(board = "ESP8266")%>%
  select(-c(new_name))%>%
  rename(sensor = updated_name)

#rename ESP32
sensor_list_redone_ESP32<-sensor_list_redone %>%
  filter(!(grepl(".",new_name,fixed=TRUE)))%>%
  mutate(board = "ESP32")%>%
  rename(sensor = new_name)


sensor_list_redone <- rbind(sensor_list_redone_ESP32,sensor_list_redone_ESP8266)

reordered_sensor_list <- sensor_list_redone %>%
  arrange(sensor)%>%
  group_by(sensor)%>%
  mutate(id = seq_along(sensor))


plot_me_new_var <- right_join(reordered_sensor_list,plot_me_slice,by=c("name","replicate"))

cleaned_data <- plot_me_new_var %>%
  select(sensor,id,rec_time,value)

sensor_list <- unique(cleaned_data$sensor)

data_for_subsetting <- cleaned_data %>%
  pivot_wider(names_from = c(sensor,id),values_from=value)


#number_of_plots = (nrow(test_scroll)-102)
number_of_plots = 4687
length_of_plots = 100

for (i in sensor_list) {
  
for (x in 1:number_of_plots) {
  
  padded_frame_no <-str_pad(as.character(x), 7, 'left', 0)
  
  current_plot_df <- data_for_subsetting[(1+x):(length_of_plots+x),]
  
  current_plot_df_plot <- current_plot_df%>%
    pivot_longer(-rec_time, names_to = c("sensor", "replicate"),
                 names_sep = "_",values_to = "reading")
  
 
    sensor_find = paste0(i,"|Eutronics")
  
  ggplot(current_plot_df_plot,
         aes(x = rec_time,
             y = reading,
             group=interaction(sensor, replicate)))+
    geom_line(aes(color=sensor))+
    #gghighlight(grepl(i,sensor))+
    gghighlight(grepl(sensor_find,sensor))+
    
    coord_cartesian(ylim = c(20,28))+
    theme_classic()+
    theme(legend.position = "none",
          axis.text.x=element_blank(),
          axis.ticks.x=element_blank(),
          axis.line=element_blank())+
    labs(x=NULL,y="Temp C")
  ggsave(
    file = paste(/,i,"_",padded_frame_no,".jpg",sep = ""),
    bg = "transparent",
    width = 4,
    height = 4)
    
  #progress = paste("progress: ",x,sep="")
  print(i)
  print(x)
}
  
}



















  
