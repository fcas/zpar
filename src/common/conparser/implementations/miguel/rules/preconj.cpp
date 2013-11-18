
//"NP|NP-TMP|NP-ADV|NX|NML < (PDT|CC|DT=target < /^(?i:either|neither|both)$/ $++ CC)",
     bool preconj1(){
     	if (node.constituent==PENN_CON_NP ||node.constituent==PENN_CON_NX){
     		CStateNodeList* childsNp=node.m_umbinarizedSubNodes;
     		while(childsNp!=0){
     			const CStateNode* pdtTarg=childsNp->node;
     			if ((((*words)[pdtTarg->lexical_head].tag.code()==PENN_TAG_PDT) || ((*words)[pdtTarg->lexical_head].tag.code()==PENN_TAG_CC) || ((*words)[pdtTarg->lexical_head].tag.code()==PENN_TAG_DT)) && !(isLinked(&node, pdtTarg))){
     				CStateNodeList* childsPdt=pdtTarg->m_umbinarizedSubNodes;
     				while(childsPdt!=0){
     					if (((*words)[childsPdt->node->lexical_head].word==g_word_either)||((*words)[childsPdt->node->lexical_head].word==g_word_neither)||((*words)[childsPdt->node->lexical_head].word==g_word_both)) {
     						CStateNodeList* rightSistersPdt=childsNp;
     						while(rightSistersPdt!=0){
     							if ((*words)[rightSistersPdt->node->lexical_head].tag.code()==PENN_TAG_CC) {
     								CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_PRECONJ);
         							if (buildStanfordLink(label, pdtTarg->lexical_head, node.lexical_head)) {
         								addLinked(&node,pdtTarg);
         								return true;
         							}
     								
     							}
     							rightSistersPdt=rightSistersPdt->next;
     						}
     						
     					}
     					childsPdt=childsPdt->next;	
     				}
     			
     		}
     		childsNp=childsNp->next;
     	}
     }
     return false;
    }

     
     //"NP|NP-TMP|NP-ADV|NX|NML < (CONJP=target < (RB < /^(?i:not)$/) < (RB|JJ < /^(?i:only|merely|just)$/) $++ CC|CONJP)",
     bool preconj2(){
    	 if (node.constituent==PENN_CON_NP || node.constituent==PENN_CON_NX){
    		 CStateNodeList* childsNp=node.m_umbinarizedSubNodes;
    		 while(childsNp!=0){
    			 const CStateNode* conjpTarg=childsNp->node;
    			 if (conjpTarg->constituent==PENN_CON_CONJP && !(isLinked(&node,conjpTarg))){
    				 bool secCond=false;
    				 bool thirdCond=false;
    				 bool fourthCond=false;
    				 CStateNodeList* rightSisters=childsNp->next;
    				 while(rightSisters!=0){
    					 if (rightSisters->node->constituent==PENN_CON_CONJP ||(*words)[rightSisters->node->lexical_head].tag.code()==PENN_TAG_CC){
    						 fourthCond=true;
    					 }
    					 rightSisters=rightSisters->next;
    				 }
    				 if (fourthCond){
    					 CStateNodeList* childsConjp=conjpTarg->m_umbinarizedSubNodes;
    					 while(childsConjp!=0){
    						 //PENN_TAG_ADJECTIVE
    						 if ((*words)[childsConjp->node->lexical_head].tag.code()==PENN_TAG_ADVERB||(*words)[childsConjp->node->lexical_head].tag.code()==PENN_TAG_ADJECTIVE){
    							 CStateNodeList* childsRB=childsConjp->node->m_umbinarizedSubNodes;
    							 while(childsRB!=0){
    								 if (((*words)[childsRB->node->lexical_head].word==g_word_not)||((*words)[childsRB->node->lexical_head].word==g_word_merely)||((*words)[childsRB->node->lexical_head].word==g_word_just)){
    									 secCond=true;
    								 }
    								 childsRB=childsRB->next;
    							 }
    						 }
    						 childsConjp=childsConjp->next;
    					 }
    					 if (secCond){
    						 childsConjp=conjpTarg->m_umbinarizedSubNodes;
    						 while(childsConjp!=0){
    							 if ((*words)[childsConjp->node->lexical_head].tag.code()==PENN_TAG_ADVERB){
    								 CStateNodeList* childsRB=childsConjp->node->m_umbinarizedSubNodes;
    								 while(childsRB!=0){
    									 if (((*words)[childsRB->node->lexical_head].word==g_word_not)){
    										 thirdCond=true;
    									 }
    									 childsRB=childsRB->next;
    								 }
    						     }
    							 childsConjp=childsConjp->next;
    						 }
    					 }
    				 }
    				 
    				 
    				 if (secCond && thirdCond && fourthCond){
    					 CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_PRECONJ);
    					 if (buildStanfordLink(label, conjpTarg->lexical_head, node.lexical_head)) {
    						 addLinked(&node,conjpTarg);
    					     return true;
    					 }
    				 }
    			 }
    			 childsNp=childsNp->next;
    			 
    		 }
    	 }
    	 return false;
     }


     //"NP|NP-TMP|NP-ADV|NX|NML < (PDT|CC|DT=target < /^(?i:either|neither|both)$/ ) < (NP < CC)",
     bool preconj3(){
    	 if (node.constituent==PENN_CON_NP||node.constituent==PENN_CON_NX){
    		 bool lastCondition=false;
    		 CStateNodeList* childsNP=node.m_umbinarizedSubNodes;
    		 while(childsNP!=0){
    			 if (childsNP->node->constituent==PENN_CON_NP){
    				 CStateNodeList* childsL=childsNP->node->m_umbinarizedSubNodes;
    				 while(childsL!=0){
    					 if ((*words)[childsL->node->lexical_head].tag.code()==PENN_TAG_CC){
    						 lastCondition=true;
    					 }
    					 childsL=childsL->next;
    				 }
    			 }
    			 childsNP=childsNP->next;
    		 }

    		 childsNP=node.m_umbinarizedSubNodes;
    		 while(childsNP!=0){
    			 const CStateNode* targ=childsNP->node;
    			 if (((*words)[targ->lexical_head].tag.code()==PENN_TAG_CC
    					 ||(*words)[targ->lexical_head].tag.code()==PENN_TAG_DT||
    					 (*words)[targ->lexical_head].tag.code()==PENN_TAG_PDT) && !isLinked(&node,targ)){
    				 CStateNodeList* childsTarg=targ->m_umbinarizedSubNodes;
    				 while(childsTarg!=0){
    					 if ((*words)[childsTarg->node->lexical_head].word==g_word_either ||
    							 (*words)[childsTarg->node->lexical_head].word==g_word_neither||
    							 (*words)[childsTarg->node->lexical_head].word==g_word_both){
    						 CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_PRECONJ);
    						 if (buildStanfordLink(label, targ->lexical_head, node.lexical_head)) {
    							 addLinked(&node,targ);
    						     return true;
    						 }

    					 }
    					 childsTarg=childsTarg->next;
    				 }
    			 }
    			 childsNP=childsNP->next;
    		 }
    	 }
    	 return false;
     }

     //"/^S|VP|ADJP|PP|ADVP|UCP(?:-TMP|-ADV)?|NX|NML|SBAR$/ < (PDT|DT|CC=target < /^(?i:either|neither|both)$/ $++ CC)",
     bool preconj4(){
    	 if (node.constituent==PENN_CON_S ||node.constituent==PENN_CON_VP ||node.constituent==PENN_CON_ADJP ||
    			 node.constituent==PENN_CON_PP ||node.constituent==PENN_CON_ADVP ||node.constituent==PENN_CON_UCP||
    		 node.constituent==PENN_CON_NX ||node.constituent==PENN_CON_SBAR){
    		CStateNodeList* childsH=node.m_umbinarizedSubNodes;
    		while(childsH!=0){
    			const CStateNode* targ=childsH->node;
				if (((*words)[targ->lexical_head].tag.code()==PENN_TAG_PDT
						||(*words)[targ->lexical_head].tag.code()==PENN_TAG_DT
						||(*words)[targ->lexical_head].tag.code()==PENN_TAG_CC) && !isLinked(&node,targ)){
					bool sisterCond=false;
					CStateNodeList* rightSisters=childsH->next;
					while(rightSisters!=0){
						if ((*words)[rightSisters->node->lexical_head].tag.code()==PENN_TAG_CC) {
							sisterCond=true;
						}
						rightSisters=rightSisters->next;
					}

					if (sisterCond){
						CStateNodeList* childsDt=targ->m_umbinarizedSubNodes;
						while(childsDt!=0){
							if (((*words)[childsDt->node->lexical_head].word==g_word_either)
									||((*words)[childsDt->node->lexical_head].word==g_word_neither)
									||((*words)[childsDt->node->lexical_head].word==g_word_both)){
								CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_PRECONJ);
								if (buildStanfordLink(label, targ->lexical_head, node.lexical_head)) {
									addLinked(&node,targ);
								    return true;
								}
							}
							childsDt=childsDt->next;
						}
					}
				}
    			childsH=childsH->next;
    		}
    	 }
    	 return false;
     }



     //"/^S|VP|ADJP|PP|ADVP|UCP(?:-TMP|-ADV)?|NX|NML|SBAR$/ < (CONJP=target < (RB < /^(?i:not)$/) < (RB|JJ < /^(?i:only|merely|just)$/) $++ CC|CONJP)"
     bool preconj5(){
    	 if (node.constituent==PENN_CON_S ||node.constituent==PENN_CON_VP ||node.constituent==PENN_CON_ADJP ||
    	    			 node.constituent==PENN_CON_PP ||node.constituent==PENN_CON_ADVP ||node.constituent==PENN_CON_UCP||
    	    		 node.constituent==PENN_CON_NX ||node.constituent==PENN_CON_SBAR){

    		 CStateNodeList* childsH=node.m_umbinarizedSubNodes;
    		 while(childsH!=0){
    			 const CStateNode* targ=childsH->node;
    			 if (targ->constituent==PENN_CON_CONJP){
    				 bool secCond=false;
    				 bool thirdCond=false;
    				 bool fourthCond=false;
    				 CStateNodeList* rightSisters=childsH->next;
    				 while(rightSisters!=0){
    					 if ((*words)[rightSisters->node->lexical_head].tag.code()==PENN_TAG_CC) {
    						 fourthCond=true;
    				 	}
    				 	rightSisters=rightSisters->next;
    				 }

    				 if (fourthCond) {
    					 CStateNodeList* childsT=targ->m_umbinarizedSubNodes;

    					 while(childsT!=0){
    						 if ((*words)[childsT->node->lexical_head].tag.code()==PENN_TAG_ADVERB) {
    							 CStateNodeList* childsRb=childsT->node->m_umbinarizedSubNodes;
    							 while(childsRb!=0){
    								 if ((*words)[childsRb->node->lexical_head].word==g_word_not){
    									 secCond=true;
    								 }
    								 childsRb=childsRb->next;
    							 }
    						 }
    					 	 childsT=childsT->next;
    				 	 }
    					 if (secCond){
    						 childsT=targ->m_umbinarizedSubNodes;
    						 while(childsT!=0){
    							 if ((*words)[childsT->node->lexical_head].tag.code()==PENN_TAG_ADVERB || (*words)[childsT->node->lexical_head].tag.code()==PENN_TAG_ADJECTIVE) {
    								 CStateNodeList* childsRb=childsT->node->m_umbinarizedSubNodes;
    						     	 while(childsRb!=0){
    						     		 if ((*words)[childsRb->node->lexical_head].word==g_word_only|| (*words)[childsRb->node->lexical_head].word==g_word_merely || (*words)[childsRb->node->lexical_head].word==g_word_just){
    						     			 thirdCond=true;
    						     		 }
    						     		 childsRb=childsRb->next;
    						     	 }
    						      }
    						     childsT=childsT->next;
    						 }
    					 }
    				 }
    				 if (secCond && thirdCond && fourthCond){
    					 CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_PRECONJ);
    					 if (buildStanfordLink(label, targ->lexical_head, node.lexical_head)) {
    						 addLinked(&node,targ);
    					 	 return true;
    					 }
    				 }
    			 }
    			 childsH=childsH->next;
    		 }
    	 }
    	 return false;
     }






