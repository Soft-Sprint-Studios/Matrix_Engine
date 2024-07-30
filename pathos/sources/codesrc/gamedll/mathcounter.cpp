#include "includes.h"
#include "gd_includes.h"
#include "mathcounter.h"

// Default values
const int CMathCounter::DEFAULT_START_VALUE = 0;
const int CMathCounter::DEFAULT_INCREMENT = 1;
const int CMathCounter::DEFAULT_MAX_VALUE = 100;
const int CMathCounter::DEFAULT_MIN_VALUE = 0;

// Link the entity to its class
LINK_ENTITY_TO_CLASS(math_counter, CMathCounter);

//=============================================
// @brief Constructor
//=============================================
CMathCounter::CMathCounter(edict_t* pedict) :
    CPointEntity(pedict),
    m_value(DEFAULT_START_VALUE),
    m_increment(DEFAULT_INCREMENT),
    m_maxValue(DEFAULT_MAX_VALUE),
    m_minValue(DEFAULT_MIN_VALUE),
    m_target(nullptr)
{
}

//=============================================
// @brief Destructor
//=============================================
CMathCounter::~CMathCounter(void)
{
}

//=============================================
// @brief Declare save fields
//=============================================
void CMathCounter::DeclareSaveFields(void)
{
    // Call base class to do it first
    CPointEntity::DeclareSaveFields();

    DeclareSaveField(DEFINE_DATA_FIELD(CMathCounter, m_value, EFIELD_INT32));
    DeclareSaveField(DEFINE_DATA_FIELD(CMathCounter, m_increment, EFIELD_INT32));
    DeclareSaveField(DEFINE_DATA_FIELD(CMathCounter, m_maxValue, EFIELD_INT32));
    DeclareSaveField(DEFINE_DATA_FIELD(CMathCounter, m_minValue, EFIELD_INT32));
    DeclareSaveField(DEFINE_DATA_FIELD(CMathCounter, m_target, EFIELD_STRING));
}

//=============================================
// @brief Handle key-value pairs
//=============================================
bool CMathCounter::KeyValue(const keyvalue_t& kv)
{
    if (!qstrcmp(kv.keyname, "startvalue"))
    {
        m_value = SDL_atoi(kv.value);
        return true;
    }
    else if (!qstrcmp(kv.keyname, "increment"))
    {
        m_increment = SDL_atoi(kv.value);
        return true;
    }
    else if (!qstrcmp(kv.keyname, "maxvalue"))
    {
        m_maxValue = SDL_atoi(kv.value);
        return true;
    }
    else if (!qstrcmp(kv.keyname, "minvalue"))
    {
        m_minValue = SDL_atoi(kv.value);
        return true;
    }
    else if (!qstrcmp(kv.keyname, "target"))
    {
        m_target = kv.value;
        return true;
    }
    else
        return CPointEntity::KeyValue(kv);
}

//=============================================
// @brief Spawn the entity
//=============================================
bool CMathCounter::Spawn(void)
{
    if (!CPointEntity::Spawn())
        return false;

    // Initialize the value if necessary
    if (m_value < m_minValue)
        m_value = m_minValue;
    else if (m_value > m_maxValue)
        m_value = m_maxValue;

    // Handle spawn flags
    if (HasSpawnFlag(FL_COUNT_ONLY))
    {
        // Only count, don't trigger targets
        m_target.clear();
    }

    return true;
}

//=============================================
// @brief Increase the counter value
//=============================================
void CMathCounter::IncrementValue(void)
{
    if (HasSpawnFlag(FL_COUNT_ONLY))
    {
        // Just increment without triggering targets
        m_value += m_increment;
        if (m_value > m_maxValue)
            m_value = m_maxValue;
        else if (m_value < m_minValue)
            m_value = m_minValue;
        return;
    }

    m_value += m_increment;

    if (HasSpawnFlag(FL_TRIGGER_ON_MAX) && m_value > m_maxValue)
    {
        m_value = m_maxValue;
        // Trigger targets using UseTargets
        UseTargets(m_activator, USE_TOGGLE, 0);
    }
    else if (HasSpawnFlag(FL_TRIGGER_ON_MIN) && m_value < m_minValue)
    {
        m_value = m_minValue;
        // Trigger targets using UseTargets
        UseTargets(m_activator, USE_TOGGLE, 0);
    }
}

//=============================================
// @brief Call the use function
//=============================================
void CMathCounter::CallUse(CBaseEntity* pActivator, CBaseEntity* pCaller, usemode_t useMode, Float value)
{
    // Set the activator (usually the entity that triggered the action)
    m_activator = pActivator;

    switch (useMode)
    {
    case USE_ON:
    case USE_OFF:
    case USE_TOGGLE:
    default:
        IncrementValue();
        if (HasSpawnFlag(FL_RESET_ON_USE))
        {
            m_value = DEFAULT_START_VALUE;
        }
        break;
    }
}
