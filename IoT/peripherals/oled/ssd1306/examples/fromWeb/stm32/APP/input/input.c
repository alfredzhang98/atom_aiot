#include "input.h"

u8 TIM5_CH1_CAPTURE_STA=0; //���벶���״̬
u32 TIM5_CH1_CAPTURE_VAL=0;//���벶��ֵ(TIM2/TIM5��32λ)

void TIM5_CH1_Input_Init(u32 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//ʹ��TIM5ʱ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);//�ܽŸ���
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//�ܽ�����
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF; //�������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//�ٶ�Ϊ100M
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ���ṹ��
	
	
	TIM_TimeBaseInitStructure.TIM_Period=arr;   //�Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; //��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //�������ϼ���ģʽ
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	
	
	TIM_ICInitStructure.TIM_Channel=TIM_Channel_1; //ͨ��1
	TIM_ICInitStructure.TIM_ICFilter=0x00;  //�˲�
	TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//������
	TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1; //��Ƶϵ��
	TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;//ֱ��ӳ�䵽TI1
	TIM_ICInit(TIM5,&TIM_ICInitStructure);
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
		
	TIM_Cmd(TIM5,ENABLE); //ʹ�ܶ�ʱ��
}

void TIM5_IRQHandler(void)
{
	if((TIM5_CH1_CAPTURE_STA&0x80)==0) //��δ�ɹ�����
	{
		if(TIM_GetITStatus(TIM5,TIM_IT_Update)) //���������ж�
		{
			if(TIM5_CH1_CAPTURE_STA&0X40)//�����˸ߵ�ƽ
			{
				if((TIM5_CH1_CAPTURE_STA&0x3f)==0x3f) //�ߵ�ƽʱ��̫��
				{
					TIM5_CH1_CAPTURE_STA|=0x80; //��־һ�β���ɹ�
					TIM5_CH1_CAPTURE_VAL=0xffffffff;
				}
				else
				{
					TIM5_CH1_CAPTURE_STA++;
				}
			}
		}
		if(TIM_GetITStatus(TIM5,TIM_IT_CC1)) //���������ж�
		{
			if(TIM5_CH1_CAPTURE_STA&0X40)//�����˵͵�ƽ
			{
				TIM5_CH1_CAPTURE_STA|=0x80; //�ɹ�����һ�θߵ�ƽ
				TIM5_CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //���������ز���
			}
			else
			{
				TIM5_CH1_CAPTURE_STA=0;
				TIM5_CH1_CAPTURE_VAL=0;
				TIM5_CH1_CAPTURE_STA|=0x40; //���񵽸ߵ�ƽ ��־
				TIM_Cmd(TIM5,DISABLE);
				TIM_SetCounter(TIM5,0); //��ʱ����ֵΪ0
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling); //�����½��ز���
				TIM_Cmd(TIM5,ENABLE);
			}
		}
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_CC1|TIM_IT_Update);
}

