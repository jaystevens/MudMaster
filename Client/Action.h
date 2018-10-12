/**************************************************************************
*  Copyright (c) 1997-2004, Kevin Cook and Aaron O'Neil
*  All rights reserved.

*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions are 
*  met:
*  
*      * Redistributions of source code must retain the above copyright 
*        notice, this list of conditions and the following disclaimer. 
*      * Redistributions in binary form must reproduce the above copyright 
*        notice, this list of conditions and the following disclaimer in 
*        the documentation and/or other materials provided with the 
*        distribution. 
*      * Neither the name of the MMGUI Project nor the names of its 
*        contributors may be used to endorse or promote products derived 
*        from this software without specific prior written permission. 
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
*  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
*  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
*  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
*  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
*  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
*  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************/
#pragma once

/**
  * \file
  * \brief Contains the Interface of the Action Class
  */

#include "TriggerImpl.h"
#include <boost/shared_ptr.hpp>

class CSession;

/**
  * \brief Contains all of the scripting entities 
  */
namespace MMScript
{
	/**
	* An action is a trigger that executes when it finds a match in the incoming text.
	*/
	class CAction
		: public CTriggerImpl<CAction>
	{
	private:
		/**
		* Create's an Action entity
		* \param action the text representing the action to take if the 
		trigger is matched.
		* \param trigger the text representing the trigger to match on.
		* \param group the name of the group to which this action belongs.
		*/
		CAction( 
			const std::string &action, 
			const std::string &trigger,
			const std::string &group );

	public:
		/** Defines a type that will be used to manage action pointers */
		typedef boost::shared_ptr<CAction> PtrType;

		/**
		  * Create's a copy of an Action entity.
		  * \param src the action to copy
		  */
		CAction(const CAction& src);

		/** Destroys an Action entity */
		~CAction();

		/** Assignment operator */
		CAction& operator=(const CAction& src);

		/**
		  * Maps an action to a textual representation
		  * \param bIncludeGroup if true the textual representation will
		           include the group to which this Action belongs.
	      */
		const std::string MapToText( BOOL bIncludeGroup );

		/** Maps an action to a MudMaster command **/
		const std::string MapToCommand();

		/** returns the "actions to take" portion of this Action entity. */
		const std::string &GetAction() const;

		/** returns the "trigger to match" portion of this Action entity. */
		const std::string &GetTrigger() const;

		/** returns the name of the group to which this action belongs. */
		const std::string &GetGroup() const;

		/** returns true if the action is enabled */
		bool IsEnabled() const;

		/** Sets the "trigger to match" for this Action entity */
		void SetTrigger( LPCTSTR trigger );

		/** Sets the "action to take" for this Action entity */
		void SetAction( LPCTSTR strAction );

		/** Sets the group to which this Action entity belongs */
		void SetGroup( LPCTSTR groupName );

		/**
		  * Sets the enabled state of this action
		  * \param enabled if true the action is enabled
		  */
		void SetEnabled( bool enabled );

		/** swaps the contents of one action with that of another */
		void Swap( CAction &action );

		/** 
		  * This is an action factory
		  * \param strAction the "action to take" for the new Action entity.
		  * \param trigger the "trigger to match" for the new Action entity.
		  * \param group the group to which the new action will belong.
		  * \param action the pointer to which the new Action entity will be assigned.
		  */
		static bool CreateAction( 
			const std::string &strAction,
			const std::string &trigger,
			const std::string &group,
			PtrType &action);

	private:
		std::string _strTrigger; /**< the "text to match" for this Action entity. */
		std::string _strAction;  /**< the "action to take" for this Action entity. */
		std::string _strGroup;   /**< the group to which this Action entity belongs. */
		bool _enabled;			 /**< the enabled state of the Action entity.  True = enabled */
	};

	/** 
	  * Compares an action to a trigger for equality
	  * \param action the action to compare
	  * \param trigger the trigger to compare
	  * \return true if the trigger matches the Action entities trigger
	  */
	inline bool operator==(const CAction::PtrType action, const std::string &trigger)
	{
		return action->GetTrigger() == trigger;
	}

	/**
	  * Checks if an Action entity is "less than" a trigger.  Alphabetic compare.
	  * \param action the action to compare
	  * \param trigger the trigger to compare
	  * \return true if the action is less than the trigger
	  */
	inline bool operator<(const CAction::PtrType action, const std::string &trigger)
	{
		return action->GetTrigger() < trigger;
	}

	/**
	  * Checks if a trigger is "less than" an Action entity.  Alphabetic compare.
	  * \param trigger the trigger to compare
	  * \param action the action to compare
	  * \return true if the trigger is less than the action
	  */
	inline bool operator<(const std::string &trigger, const CAction::PtrType action)
	{
		return trigger < action->GetTrigger();
	}
}
