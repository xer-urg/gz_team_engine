#pragma once

class CWound;
class NET_Packet;
class CEntityAlive;
class CLevel;

#include "hit_immunity.h"
#include "Hit.h"
#include "Level.h"

enum EBoostParams{
	eBoostHpRestore = 0,
	eBoostPowerRestore,
	eBoostRadiationRestore,
	eBoostBleedingRestore,
	eBoostMaxWeight,
	eBoostRadiationProtection,
	eBoostTelepaticProtection,
	eBoostChemicalBurnProtection,
	eBoostBurnImmunity,
	eBoostShockImmunity,
	eBoostRadiationImmunity,
	eBoostTelepaticImmunity,
	eBoostChemicalBurnImmunity,
	eBoostExplImmunity,
	eBoostStrikeImmunity,
	eBoostFireWoundImmunity,
	eBoostWoundImmunity,
	eBoostMaxCount,
};

static const LPCSTR ef_boosters_section_names[] =
{
	"boost_health_restore",
	"boost_power_restore",
	"boost_radiation_restore",
	"boost_bleeding_restore",
	"boost_max_weight",
	"boost_radiation_protection",
	"boost_telepat_protection",
	"boost_chemburn_protection",
	"boost_burn_immunity",
	"boost_shock_immunity",
	"boost_radiation_immunity",
	"boost_telepat_immunity",
	"boost_chemburn_immunity",
	"boost_explosion_immunity",
	"boost_strike_immunity",
	"boost_fire_wound_immunity",
	"boost_wound_immunity"
};

struct SBooster{
	float fBoostTime;
	float fBoostValue;
	EBoostParams m_type;
	SBooster():fBoostTime(-1.0f){};
	void Load(const shared_str& sect, EBoostParams type);
};

class CEntityCondition;
class CEntityConditionSimple
{
	float					m_fHealth;
	float					m_fHealthMax;
public:
							CEntityConditionSimple	();
	virtual					~CEntityConditionSimple	();

	IC float				GetHealth				() const			{return m_fHealth;}
	IC float 				GetMaxHealth			() const			{return m_fHealthMax;}
	IC float&				health					()					{return	m_fHealth;}
	IC float&				max_health				()					{return	m_fHealthMax;}

	virtual	CEntityCondition*	cast_entity_condition()					{ return NULL;  }
};

class CEntityCondition: public CEntityConditionSimple, public CHitImmunity
{
private:
	bool					m_use_limping_state;
	CEntityAlive			*m_object;

public:
							CEntityCondition		(CEntityAlive *object);
	virtual					~CEntityCondition		(void);

	virtual void			LoadCondition			(LPCSTR section);
	virtual void			remove_links			(const CObject *object);

	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

	IC float				GetPower				() const			{return m_fPower;}	
	IC float				GetRadiation			() const			{return m_fRadiation;}
	IC float				GetPsyHealth			() const			{return m_fPsyHealth;}

	IC float 				GetEntityMorale			() const			{return m_fEntityMorale;}

	IC float 				GetHealthLost			() const			{return m_fHealthLost;}

	virtual bool 			IsLimping				() const;

	virtual float				GetSatiety			()					{return 0.f;};
	virtual float            	GetThirst			()					{return 0.f;};
	virtual float xr_stdcall	GetAlcohol			()					{return 0.f;};

	virtual void 			ChangeSatiety			(float value)		{};
	virtual void 			ChangeThirst			(float value)		{};
	virtual void 			ChangeAlcohol			(float value)		{};
	void 					ChangeHealth			(float value);
	void 					ChangePower				(float value);
	void 					ChangeRadiation			(float value);
	void 					ChangePsyHealth			(float value);
	
	IC void					MaxPower				()					{m_fPower = m_fPowerMax;};
	IC void					SetMaxPower				(float val)			{m_fPowerMax = val; clamp(m_fPowerMax,0.1f,1.0f);};
	IC float				GetMaxPower				() const			{return m_fPowerMax;};

	void 					ChangeBleeding			(float percent);

	void 					ChangeCircumspection	(float value);
	void 					ChangeEntityMorale		(float value);

	virtual CWound*			ConditionHit			(SHit* pHDS);
	//���������� ��������� � �������� �������
	virtual void			UpdateCondition			();
	void					UpdateWounds			();
	void					UpdateConditionTime		();
	IC void					SetConditionDeltaTime	(float DeltaTime) { m_fDeltaTime = DeltaTime; };

	
	//�������� ������ ����� �� ���� �������� ��� 
	float					BleedingSpeed			();

	CObject*				GetWhoHitLastTime		() {return m_pWho;}
	u16						GetWhoHitLastTimeID		() {return m_iWhoID;}

	virtual bool			ApplyBooster			(const SBooster& B, const shared_str& sect);

	CWound*					AddWound				(float hit_power, ALife::EHitType hit_type, u16 element);

	IC void 				SetCanBeHarmedState		(bool CanBeHarmed) 			{m_bCanBeHarmed = CanBeHarmed;}
	IC bool					CanBeHarmed				() const					{return OnServer() && m_bCanBeHarmed;};
	
	void					ClearWounds();

	IC float				GetBoostRadiationImmunity() const {return m_fBoostRadiationImmunity;};
	typedef					xr_map<EBoostParams, SBooster> BOOSTER_MAP;
protected:
	void					UpdateHealth			();
	void					UpdatePower				();
	void					UpdateSatiety			(float k=1.0f);
	void					UpdateRadiation			(float k=1.0f);
	void					UpdatePsyHealth			(float k=1.0f);

	void					UpdateEntityMorale		();


	//��������� ���� ���� � ����������� �� �������� �������
	//(������ ��� InventoryOwner)
	float					HitOutfitEffect			(float hit_power, ALife::EHitType hit_type, s16 element, float AP);
	//��������� ������ ��� � ����������� �� �������� �������
	float					HitPowerEffect			(float power_loss);
	
	//��� �������� ��������� �������� ���,
	//������������ ����� ���� ��� ������� ���
	//� �������� ������ ����� �� ����
	DEFINE_VECTOR(CWound*, WOUND_VECTOR, WOUND_VECTOR_IT);
	WOUND_VECTOR			m_WoundVector;
	//������� ������� ���
	

	//��� �������� �� 0 �� 1			
	float m_fPower;					//����
	float m_fRadiation;				//���� ������������� ���������
	float m_fPsyHealth;				//��������

	float m_fEntityMorale;			//������

	//������������ ��������
	//	float m_fSatietyMax;
	float m_fPowerMax;
	float m_fRadiationMax;
	float m_fPsyHealthMax;

	float m_fEntityMoraleMax;

	//�������� ��������� ���������� �� ������ ����������
	float m_fDeltaHealth;
	float m_fDeltaPower;
	float m_fDeltaRadiation;
	float m_fDeltaPsyHealth;

	float m_fDeltaCircumspection;
	float m_fDeltaEntityMorale;

	struct SConditionChangeV
	{
const	static int		PARAMS_COUNT = 7;

		float			m_fV_Radiation;
		float			m_fV_PsyHealth;
		float			m_fV_Circumspection;
		float			m_fV_EntityMorale;
		float			m_fV_RadiationHealth;
		float			m_fV_Bleeding;
		float			m_fV_WoundIncarnation;
		float			m_fV_HealthRestore;
		float			&value(LPCSTR name);
		void			load(LPCSTR sect, LPCSTR prefix);
	};
	
	SConditionChangeV m_change_v;

	float				m_fMinWoundSize;
	bool				m_bIsBleeding;

	//����� ����, ������������� �� ���������� �������� � ����
	float				m_fHealthHitPart;
	float				m_fPowerHitPart;

	float				m_fBoostBurnImmunity;
	float				m_fBoostShockImmunity;
	float				m_fBoostRadiationImmunity;
	float 				m_fBoostTelepaticImmunity;
	float 				m_fBoostChemicalBurnImmunity;
	float 				m_fBoostExplImmunity;
	float 				m_fBoostStrikeImmunity;
	float 				m_fBoostFireWoundImmunity;
	float 				m_fBoostWoundImmunity;
	float 				m_fBoostRadiationProtection;
	float 				m_fBoostTelepaticProtection;
	float 				m_fBoostChemicalBurnProtection;

	//������ �������� �� ���������� ����
	float				m_fHealthLost;


	//��� ������������ ������� 
	u64					m_iLastTimeCalled;
	float				m_fDeltaTime;
	//��� ����� ��������� ���
	CObject*			m_pWho;
	u16					m_iWhoID;

	//��� �������� ���������� �� DamageManager
	float				m_fHitBoneScale;
	float				m_fWoundBoneScale;

	float				m_limping_threshold;

	bool				m_bTimeValid;
	bool				m_bCanBeHarmed;
	BOOSTER_MAP			m_booster_influences;

public:
	virtual void					reinit				();
	
	IC const	float				fdelta_time			() const 	{return		(m_fDeltaTime);			}
	IC const	WOUND_VECTOR&		wounds				() const	{return		(m_WoundVector);		}
	IC float&						radiation			()			{return		(m_fRadiation);			}
	IC float&						hit_bone_scale		()			{return		(m_fHitBoneScale);		}
	IC float&						wound_bone_scale	()			{return		(m_fWoundBoneScale);	}
	virtual	CEntityCondition*		cast_entity_condition()					{ return this; }
	static  void					script_register(lua_State *L);
	virtual float					GetParamByName		(LPCSTR name);
};
