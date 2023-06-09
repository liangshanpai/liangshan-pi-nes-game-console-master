#include "rtc.h"
#include <stdio.h>
#include "usart0.h"





rtc_parameter_struct   rtc_initpara;
rtc_alarm_struct  rtc_alarm;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;



uint8_t usart_input_threshold(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2){
        while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_RBNE));
        tmp[index++] = usart_data_receive(EVAL_COM0);
        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){
            printf("\n\r please input a valid number between 0 and 9 \n\r");
            index--;
        }
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    if (index > value){
        printf("\n\r please input a valid number between 0 and %d \n\r", value);
        return 0xFF;
    }

    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) <<4);
    return index;
}


void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC32K)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

void rtc_setup(void)
{
    uint32_t tmp_hh = 0, tmp_mm = 0, tmp_ss = 0;
    
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = 0x16;
    rtc_initpara.day_of_week = RTC_SATURDAY;
    rtc_initpara.month = RTC_APR;
    rtc_initpara.date = 0x30;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;
    
    rtc_initpara.hour = tmp_hh;
    rtc_initpara.minute = tmp_mm;
    rtc_initpara.second = tmp_ss;
    
    if(ERROR == rtc_init(&rtc_initpara)){
        printf("\n\r** RTC time configuration failed! **\n\r");
    }else{
        printf("\n\r** RTC time configuration success! **\n\r");
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }
}



//void rtc_setup(void)
//{
//    /* setup RTC time value */
//    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

//    rtc_initpara.factor_asyn = prescaler_a;
//    rtc_initpara.factor_syn = prescaler_s;
//    rtc_initpara.year = 0x16;
//    rtc_initpara.day_of_week = RTC_SATURDAY;
//    rtc_initpara.month = RTC_APR;
//    rtc_initpara.date = 0x30;
//    rtc_initpara.display_format = RTC_24HOUR;
//    rtc_initpara.am_pm = RTC_AM;

//    /* current time input */
//    printf("=======Configure RTC Time========\n\r");
//    printf("  please input hour:\n\r");
//    while (0xFF == tmp_hh){
//        tmp_hh = usart_input_threshold(23);
//        rtc_initpara.hour = tmp_hh;
//    }
//    printf("  %0.2x\n\r", tmp_hh);

//    printf("  please input minute:\n\r");
//    while (0xFF == tmp_mm){
//        tmp_mm = usart_input_threshold(59);
//        rtc_initpara.minute = tmp_mm;
//    }
//    printf("  %0.2x\n\r", tmp_mm);

//    printf("  please input second:\n\r");
//    while (0xFF == tmp_ss){
//        tmp_ss = usart_input_threshold(59);
//        rtc_initpara.second = tmp_ss;
//    }
//    printf("  %0.2x\n\r", tmp_ss);

//    /* RTC current time configuration */
//    if(ERROR == rtc_init(&rtc_initpara)){
//        printf("\n\r** RTC time configuration failed! **\n\r");
//    }else{
//        printf("\n\r** RTC time configuration success! **\n\r");
//        rtc_show_time();
//        RTC_BKP0 = BKP_VALUE;
//    }

//    /* setup RTC alarm */
//    tmp_hh = 0xFF;
//    tmp_mm = 0xFF;
//    tmp_ss = 0xFF;

//    rtc_alarm_disable(RTC_ALARM0);
//    printf("=======Input Alarm Value=======\n\r");
//    rtc_alarm.alarm_mask = RTC_ALARM_DATE_MASK|RTC_ALARM_HOUR_MASK|RTC_ALARM_MINUTE_MASK;
//    rtc_alarm.weekday_or_date = RTC_ALARM_DATE_SELECTED;
//    rtc_alarm.alarm_day = 0x31;
//    rtc_alarm.am_pm = RTC_AM;

//    /* RTC alarm input */
//    printf("  please input Alarm Hour:\n\r");
//    while (0xFF == tmp_hh){
//        tmp_hh = usart_input_threshold(23);
//        rtc_alarm.alarm_hour = tmp_hh;
//    }
//    printf("  %0.2x\n\r", tmp_hh);

//    printf("  Please Input Alarm Minute:\n\r");
//    while (0xFF == tmp_mm){
//        tmp_mm = usart_input_threshold(59);
//        rtc_alarm.alarm_minute = tmp_mm;
//    }
//    printf("  %0.2x\n\r", tmp_mm);

//    printf("  Please Input Alarm Second:\n\r");
//    while (0xFF == tmp_ss){
//        tmp_ss = usart_input_threshold(59);
//        rtc_alarm.alarm_second = tmp_ss;
//    }
//    printf("  %0.2x", tmp_ss);

//    /* RTC alarm configuration */
//    rtc_alarm_config(RTC_ALARM0,&rtc_alarm);
//    printf("\n\r** RTC Set Alarm Success!  **\n\r");
//    rtc_show_alarm();

//    rtc_interrupt_enable(RTC_INT_ALARM0);
//    rtc_alarm_enable(RTC_ALARM0);
//}


void rtc_show_time(void)
{
    uint32_t time_subsecond = 0;
    uint8_t subsecond_ss = 0,subsecond_ts = 0,subsecond_hs = 0;

    rtc_current_time_get(&rtc_initpara);

    /* get the subsecond value of current time, and convert it into fractional format */
    time_subsecond = rtc_subsecond_get();
    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;

    printf("Current time: %0.2x:%0.2x:%0.2x .%d%d%d \n\r", \
          rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second,\
          subsecond_ss, subsecond_ts, subsecond_hs);
}


void rtc_show_alarm(void)
{
    rtc_alarm_get(RTC_ALARM0,&rtc_alarm);
    printf("The alarm: %0.2x:%0.2x:%0.2x \n\r", rtc_alarm.alarm_hour, rtc_alarm.alarm_minute,\
           rtc_alarm.alarm_second);
}




void rtctime_init(void)
{
    /* enable access to RTC registers in Backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
    
    rtc_pre_config();
    if (BKP_VALUE != RTC_BKP0)rtc_setup();

    rcu_all_reset_flag_clear();
    exti_flag_clear(EXTI_22);
    exti_init(EXTI_22,EXTI_INTERRUPT,EXTI_TRIG_RISING);
    nvic_irq_enable(RTC_WKUP_IRQn,0,0);
    rtc_flag_clear(RTC_FLAG_WT);
    rtc_interrupt_enable(RTC_INT_WAKEUP);
    rtc_wakeup_clock_set(WAKEUP_CKSPRE);
    rtc_wakeup_timer_set(0);
    rtc_wakeup_enable();
}







