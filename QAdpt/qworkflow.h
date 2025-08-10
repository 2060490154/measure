#ifndef QWORKFLOW_H
#define QWORKFLOW_H

#include <QObject>
#include "define.h"
#include "qruntaskthread.h"

class QWorkFlowThread:public QThread
{

    Q_OBJECT
public:
    explicit QWorkFlowThread(QDevManagerCtrl* pCtrl);
    ~QWorkFlowThread();

signals:
    void signal_UserChangeResult(QVariant tTaskExpInfo,bool bstatus);
public:

    bool ProcessMoveWorkFlow(QWorkFlowItem* pMoveItem);
    bool ProcessSetWorkFlow(QWorkFlowItem* pSetItem);
    bool ProcessCapWorkFlow(QWorkFlowItem* pCapItem);
	bool ProcessDataWorkFlow(QWorkFlowItem* pMoveItem);

    void clearWorkFlow(QList<QWorkFlowItem*> * pItem);

	void CreatStandbyWorkFlow(int nWavelength = 0);
    //自动流程
    void create1On1WorkFlow(int nPtsPerSectionCnt,double dMinEnergy,double dMaxEnergy,int nEnergySection,QString sExpNo,int nWaveLengthType, double dMinDeg, double dMaxDeg);
    void CreateSOn1WorkFlow(int nCnt,double lfEnergyIncream,QString sExpNo,int nWaveLengthType,int nPointsPerEnergy);
    void CreateROn1WorkFlow(double lfMinEnergy, double lfMaxEnergy, int nEnergySecNum, QString sExpNo, int nWaveLengthType, double dMinDeg, double dMaxDeg);
    void createRasterWorkFlow(int nCnt,double lfMinEnergy,double lfMaxEnergy,int nEnergySection, QString sExpNo,int nWaveLengthType);
    //手动流程
    void create1On1ManualWorkFlow(double lfEnergy,double dStep,QString sExpNo,int nWaveLengthType);
    void createAverAreaManualWorkFlow(int nCnt, int nWaveLengthType);
    void createPulsewidthWorkFlow(int nCnt, int nWaveLengthType);//yhy0320
    //New Shutter Test flow
    void createNewShutterTestWorkFlow(int nCnt);
    void createSampleStage2ZeroWorkFlow();
    void createAverEnergyRatioWorkFlow(int nCnt, int nWaveLengthType);

    int getEnergyMotorIndex(int nWaveLengthType);//根据波长选择玻片对应的电机
    int getShutterIndex(int nWaveLengthType);   //choose the shutter index based on the wavelengthtype

    //流程解析
    void ProcessStandByFlow();
    void ProcessWorkFlow();
    void ProcessManualWorkFlow();
    void ProcessAverAreaWorkFlow();
    void ProcessAverEnergyMeterWorkFlow();
    void ProcessPulsewidthWorkFlow();//yhy0320
    int waitForWorkflowRel(QList<QWorkTask>* pSteplist);
    int waitForStepRel( QWorkTask* pStep);

    void run();

public:
        QList<QWorkFlowItem*> m_pStandbyFlow;
        QList<QWorkFlowItem*> m_pWorkFlowList;
        QList<QWorkFlowItem*> m_pManualWorkFlowlist;
        QList<QWorkFlowItem*> m_pAverAreaWorkFlowlist;
        QList<QWorkFlowItem*> m_pAverEnergyMeterWorkFlowlist;
        QList<QWorkFlowItem*> m_pPulsewidthWorkFlowlist;//yhy0320
        QList<QPointInfo> m_tPointList;

        QList<QRunTaskThread*> m_tThreadList;

        bool m_bImmediateStop;
        bool m_bSuspend;
        bool m_bManualAlter;
        bool m_bCycleStatus;
        int m_nRasterCycleCnt;

        int m_nWorkFlowTYpe;//1---standby  2---workflow
        int m_nMeasureType;

        tCoeff* m_pEnergyAdjCoeff;//0-1064 1-533 2-355



private:
    QDevManagerCtrl* _pDevManagerCtrl;
    double m_dBaseEnergy;
};

#endif // QWORKFLOW_H
