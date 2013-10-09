
 //"VP < (S=target < (VP !<, TO|VBG|VBN) !$-- NP)",
    bool buildCComp1() {
    	if (node.constituent==PENN_CON_VP) {
    		CStateNodeList* childsVp=node.m_umbinarizedSubNodes;
    		while(childsVp=0){
    			const CStateNode* sTarg=childsVp->node;
    			if (sTarg->constituent==PENN_CON_S && (!isLinked(&node,sTarg))) {
    				bool inCondition=true; //!<, TO|VBG|VBN
    				//A <, B B is the first child of A
    				CStateNodeList* childsS=node.m_umbinarizedSubNodes;
    				while(childsS!=0) {
    					const CStateNode* vpChildS=childsS->node;
    					if (vpChildS->constituent==PENN_CON_VP) {
    						CStateNodeList* childsVpIn=vpChildS->m_umbinarizedSubNodes;
    						if (childsVpIn!=0) { //only the first one
    							const CStateNode* firstChild=childsVpIn->node;
    							if (((*words)[firstChild->lexical_head].tag.code()==PENN_TAG_TO)
    								 || ((*words)[firstChild->lexical_head].tag.code()==PENN_TAG_VERB_PROG)
    								 || ((*words)[firstChild->lexical_head].tag.code()==PENN_TAG_VERB_PAST_PARTICIPATE)) {

    								inCondition=false;

    							}
    						}
    					}
    					childsS=childsS->next;
    				}
    				if (inCondition) {
    					//A $-- B (A is a right sister of B)
    					bool secondCondition=true;
    					CStateNodeList* leftSistersS=childsVp->previous;
    					while(leftSistersS!=0){
    						if (leftSistersS->node->constituent==PENN_CON_NP){
    							secondCondition=false;
    						}
    						leftSistersS=leftSistersS->previous;
    					}
    					if (secondCondition) {
    						CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
    						if (buildStanfordLink(label, sTarg->lexical_head, node.lexical_head)) {
    							addLinked(&node,sTarg);
    						    return true;
    						}
    					}
    				}
    			}
    			childsVp=childsVp->next;
    		}
    	}
    	return false;
    }


    //"VP < (SBAR=target < (S <+(S) VP) <, (IN|DT < /^(?i:that|whether)$/))",
      bool buildCComp2() {

      	if (node.constituent==PENN_CON_VP){
      		CStateNodeList* childsVp=node.m_umbinarizedSubNodes;
      		while(childsVp!=0){
      			const CStateNode* sbarTarg=childsVp->node;
      			if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
      				CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
      				bool secondCondition=false;
      				if (childsSbar!=0){ //Only first child <,
      					const CStateNode* firstChildSbar=childsSbar->node;
      					if (((*words)[firstChildSbar->lexical_head].tag.code()==PENN_TAG_IN)
      						|| ((*words)[firstChildSbar->lexical_head].tag.code()==PENN_TAG_DT)){
      						CStateNodeList* childsInDt=firstChildSbar->m_umbinarizedSubNodes;
      						while(childsInDt!=0){
      							if (((*words)[childsInDt->node->lexical_head].word==g_word_whether)&&((*words)[childsInDt->node->lexical_head].word==g_word_that)) {
      								secondCondition=true;
      							}
      							childsInDt=childsInDt->next;
      						}
      					}
      				}
      				if (secondCondition){
      					while(childsSbar!=0){
      						const CStateNode* sChildSbar=childsSbar->node;
      						if (sChildSbar->constituent==PENN_CON_S){
      							//A dominates B via an unbroken chain of (zero or more) nodes matching description C
      							CStateNodeList* sChain=new CStateNodeList();
      							findChain(PENN_CON_S,PENN_CON_VP, sChildSbar, sChain);
      							while(sChain!=0){
      								if (sChain->node->constituent==PENN_CON_VP){
      									CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
      									if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
      										addLinked(&node,sbarTarg);
      										return true;
      									}
      								}
      								sChain=sChain->next;
      							}
      						}
      						childsSbar=childsSbar->next;
      					}
      				}
      			}
      			childsVp=childsVp->next;
      		}
      	}
      	return false;
      }



      //"VP < (SBAR=target < (SBAR < (S <+(S) VP) <, (IN|DT < /^(?i:that|whether)$/)) < CC|CONJP)",
         bool buildCComp3() {

            	if (node.constituent==PENN_CON_VP){
            		CStateNodeList* childsVp=node.m_umbinarizedSubNodes;
            		while(childsVp!=0){
            			const CStateNode* sbarTarg=childsVp->node;
            			if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
            				CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
            				bool secondCondition=false;
            				if (childsSbar!=0){ //Only first child <,
            					const CStateNode* firstChildSbar=childsSbar->node;
            					if (((*words)[firstChildSbar->lexical_head].tag.code()==PENN_TAG_IN)
            						|| ((*words)[firstChildSbar->lexical_head].tag.code()==PENN_TAG_DT)){
            						CStateNodeList* childsInDt=firstChildSbar->m_umbinarizedSubNodes;
            						while(childsInDt!=0){
            							if (((*words)[childsInDt->node->lexical_head].word==g_word_whether)&&((*words)[childsInDt->node->lexical_head].word==g_word_that)) {
            								secondCondition=true;
            							}
            							childsInDt=childsInDt->next;
            						}
            					}
            				}
            				bool thirdCondition=false;
            				while(childsSbar!=0){
            					if (((*words)[childsSbar->node->lexical_head].tag.code()==PENN_TAG_CC)
            					    || (childsSbar->node->constituent==PENN_CON_CONJP)){
            						thirdCondition=true;
            					}
            					childsSbar=childsSbar->next;
            				}
            				if (secondCondition && thirdCondition){
            					childsSbar=sbarTarg->m_umbinarizedSubNodes;
            					while(childsSbar!=0){
            						const CStateNode* sChildSbar=childsSbar->node;
            						if (sChildSbar->constituent==PENN_CON_S){
            							//A dominates B via an unbroken chain of (zero or more) nodes matching description C
            							CStateNodeList* sChain=new CStateNodeList();
            							findChain(PENN_CON_S,PENN_CON_VP, sChildSbar, sChain);
            							while(sChain!=0){
            								if (sChain->node->constituent==PENN_CON_VP){
            									CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
            									if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
            										addLinked(&node,sbarTarg);
            										return true;
            									}
            								}
            								sChain=sChain->next;
            							}
            						}
            						childsSbar=childsSbar->next;
            					}
            				}
            			}
            			childsVp=childsVp->next;
            		}
            	}
            	return false;
            }


//"VP < (SBAR=target < (S < VP) !$-- NP !<, (IN|WHADVP))",
 bool buildCComp4() {
 	if (node.constituent==PENN_CON_VP) {
 		CStateNodeList* childsVp=node.m_umbinarizedSubNodes;
 		while(childsVp!=0){
 			const CStateNode* sbarTarg=childsVp->node;
 			if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
 				CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
 				bool secondCondition=true;
 				bool thirdCondition=true;

 				//A $-- B A is a right sister of B
 				CStateNodeList* leftSistersSbar=childsVp->previous;
 				while(leftSistersSbar!=0){
 					if (leftSistersSbar->node->constituent==PENN_CON_NP){
 						secondCondition=false;
 					}
 					leftSistersSbar=leftSistersSbar->previous;
 				}
 				//A !<, B 	B is the first child of A
 				if (childsSbar!=0) { //only the first child
 					if (childsSbar->node->constituent==PENN_CON_WHADJP || (*words)[childsSbar->node->lexical_head].tag.code()==PENN_TAG_IN) {
 						thirdCondition=false;
 					}
 				}
 				if (secondCondition && thirdCondition) {
 					while(childsSbar!=0) {
 						const CStateNode* sChildSbar=childsSbar->node;
 						if (sChildSbar->constituent==PENN_CON_S){
 							CStateNodeList* childsS=sChildSbar->m_umbinarizedSubNodes;
 							while(childsS!=0){
 								const CStateNode* vpChildS=childsS->node;
 								if (vpChildS->constituent==PENN_CON_VP){
 									CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
 									if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
 										addLinked(&node,sbarTarg);
 									    return true;
 									}
 								}
 								childsS=childsS->next;
 							}
 						}
 						childsSbar=childsSbar->next;
 					}
 				}

 			}
 			childsVp=childsVp->next;
 		}
 	}
 	return false;

 }

 //"VP < (SBAR=target < (S < VP) !$-- NP <, (WHADVP < (WRB < /^(?i:how)$/)))",
 bool buildCComp5() {
 	if (node.constituent==PENN_CON_VP){
 		CStateNodeList* childsVp=node.m_umbinarizedSubNodes;
 		while(childsVp!=0){
 			const CStateNode* sbarTarg=childsVp->node;
 			if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))){
 				bool noNpLeftSister=true;
 				CStateNodeList* leftSistersSbar=childsVp->previous;
 				while(leftSistersSbar!=0){
 					if (leftSistersSbar->node->constituent==PENN_CON_NP){
 						noNpLeftSister=false;
 					}
 					leftSistersSbar=leftSistersSbar->previous;
 				}

 				if (noNpLeftSister){
 					CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
 					while(childsSbar!=0){
 						const CStateNode* sChildSbar=childsSbar->node;
 						if (sChildSbar->constituent==PENN_CON_S){
 							CStateNodeList* childsS=sChildSbar->m_umbinarizedSubNodes;
 							while(childsS!=0){
 								if (childsS->node->constituent==PENN_CON_VP){
 									const CStateNode* firstChildSbar=sbarTarg->m_umbinarizedSubNodes->node;
 									if (firstChildSbar->constituent==PENN_CON_WHADVP){
 										CStateNodeList* childsWhadvp=firstChildSbar->m_umbinarizedSubNodes;
 										while(childsWhadvp!=0){
 											const CStateNode* wrbChildWh=childsWhadvp->node;
 											if ((*words)[wrbChildWh->lexical_head].tag.code()==PENN_TAG_WRB){
 												CStateNodeList* childsWrb=wrbChildWh->m_umbinarizedSubNodes;
 												while(childsWrb!=0){
 													if (((*words)[childsWrb->node->lexical_head].word==g_word_how)) {
 														CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
 														if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
 															addLinked(&node,sbarTarg);
 															return true;
 														}
 													}
 													childsWrb=childsWrb->next;
 												}
 											}
 											childsWhadvp=childsWhadvp->next;
 										}
 									}
 								}
 								childsS=childsS->next;
 							}
 						}
 						childsSbar=childsSbar->next;
 					}
 				}

 			}
 			childsVp=childsVp->next;
 		}
 	}
 	return false;

 }

 //"VP < (/^VB/ < " + haveRegex + ") < (S=target < @NP < VP)",
   bool buildCComp6() {

   	if (node.constituent==PENN_CON_VP){
   		CStateNodeList* childsVp=node.m_umbinarizedSubNodes;
   		bool firstCondition=false;
   		while(childsVp!=0){
   			const CStateNode* vbChildVp=childsVp->node;
   			if (((*words)[vbChildVp->lexical_head].tag.code()==PENN_TAG_VERB)
   					|| ((*words)[vbChildVp->lexical_head].tag.code()==PENN_TAG_VERB_PAST)
   					|| ((*words)[vbChildVp->lexical_head].tag.code()==PENN_TAG_VERB_PROG)
   					|| ((*words)[vbChildVp->lexical_head].tag.code()==PENN_TAG_VERB_PAST_PARTICIPATE)
   					|| ((*words)[vbChildVp->lexical_head].tag.code()==PENN_TAG_VERB_PRES)
   					|| ((*words)[vbChildVp->lexical_head].tag.code()==PENN_TAG_VERB_THIRD_SINGLE)) {

   				CStateNodeList* childsVb=vbChildVp->m_umbinarizedSubNodes;
   				while(childsVb!=0){
   					const CStateNode* haveChildVb=childsVb->node;
   					 if (compareWordToHaveWordRegex((*words)[haveChildVb->lexical_head].word)) {
   						 firstCondition=true;
   					 }
   					childsVb=childsVb->next;
   				}
   			}
   			childsVp=childsVp->next;
   		}
   		childsVp=node.m_umbinarizedSubNodes;
   		if (firstCondition){
   			while(childsVp!=0){
   				const CStateNode* sTarg=childsVp->node;
   				if (sTarg->constituent==PENN_CON_S && (!isLinked(&node,sTarg))) {
   					CStateNodeList* childsS=sTarg->m_umbinarizedSubNodes;
   					while(childsS!=0){
   						const CStateNode* npChildS=childsS->node;
   						if (npChildS->constituent==PENN_CON_NP){
   							CStateNodeList* childsNp=npChildS->m_umbinarizedSubNodes;
   							while(childsNp!=0){
   								if (childsNp->node->constituent==PENN_CON_VP){
   									CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
   									if (buildStanfordLink(label, sTarg->lexical_head, node.lexical_head)) {
   										addLinked(&node,sTarg);
   										return true;
   									}
   								}
   								childsNp=childsNp->next;
   							}
   						}
   						childsS=childsS->next;
   					}
   				}
   				childsVp=childsVp->next;
   			}
   		}
   	}
   	return false;
   }


   //"S|SINV < (S|SBARQ=target $+ /^(,|\\.|'')$/ !$- /^(?:CC|:)$/ !< (VP < TO|VBG|VBN))",
   bool buildCComp7() {
   	if (node.constituent==PENN_CON_S||node.constituent==PENN_CON_SINV){
   		CStateNodeList* childsSSinv=node.m_umbinarizedSubNodes;
   		while(childsSSinv!=0){
   			const CStateNode* ssbarqTarg=childsSSinv->node;
   			if ((ssbarqTarg->constituent==PENN_CON_S||ssbarqTarg->constituent==PENN_CON_SBARQ) && (!isLinked(&node,ssbarqTarg))){
   				bool secondCond=true;
   				bool thirdCond=true;

   				//A $- B 	A is the immediate right sister of B
   				if (childsSSinv->previous!=0){
   					if ((*words)[childsSSinv->previous->node->lexical_head].tag.code()==PENN_TAG_CC) { //:CC, subcategory of CC...¿? how come? I match it with CC ... should be enough.
   						secondCond=false;
   					}
   				}

   				CStateNodeList* childsSSbarq=ssbarqTarg->m_umbinarizedSubNodes;
   				while(childsSSbarq!=0){
   					const CStateNode* vpChildssbarq=childsSSbarq->node;
   					if (vpChildssbarq->constituent==PENN_CON_VP){
   						CStateNodeList* childsVp=vpChildssbarq->m_umbinarizedSubNodes;
   						while(childsVp!=0){
   							if (((*words)[childsVp->node->lexical_head].tag.code()==PENN_TAG_TO)
   									&& ((*words)[childsVp->node->lexical_head].tag.code()==PENN_TAG_VERB_PROG)
   									&& ((*words)[childsVp->node->lexical_head].tag.code()==PENN_TAG_VERB_PAST_PARTICIPATE)) { //PENN_TAG_VERB_PROG, PENN_TAG_VERB_PAST_PARTICIPATE
   								thirdCond=false;

   							}
   							childsVp=childsVp->next;
   						}
   					}
   					childsSSbarq=childsSSbarq->next;
   				}

   				//A $+ B 	A is the immediate left sister of B
   				if (secondCond && thirdCond && childsSSinv->next!=0){
   					if (((*words)[childsSSinv->next->node->lexical_head].word==g_word_comma)
   							&&((*words)[childsSSinv->next->node->lexical_head].word==g_word_dot)
   							&&((*words)[childsSSinv->next->node->lexical_head].word==g_word_two_quotes)) {
   						CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
   						if (buildStanfordLink(label, ssbarqTarg->lexical_head, node.lexical_head)) {
   							addLinked(&node,ssbarqTarg);
   							return true;
   						}
   					}

   				}

   			}
   			childsSSinv=childsSSinv->next;
   		}
   	}
   	return false;
   }


//"ADVP < (SBAR=target < (IN < /^(?i:as|that)/) < (S < (VP !< TO)))"
bool buildCComp8() {
	if (node.constituent==PENN_CON_ADVP){
		CStateNodeList* childsAdvp=node.m_umbinarizedSubNodes;
		bool secondCondition=false;
		while(childsAdvp!=0){
			const CStateNode* sChildAdvp=childsAdvp->node;
			bool inCondition=true;
			CStateNodeList* childsS=sChildAdvp->m_umbinarizedSubNodes;
			while(childsS!=0){
				const CStateNode* vpChildS=childsS->node;
				if (vpChildS->constituent==PENN_CON_VP) {
					CStateNodeList* childsVp=vpChildS->m_umbinarizedSubNodes;
					while(childsVp!=0){
						const CStateNode* toChildVp=childsVp->node;
						if (((*words)[toChildVp->lexical_head].tag.code()==PENN_TAG_TO)) {
							inCondition=false;

						}
						childsVp=childsVp->next;
					}
					secondCondition=inCondition;
				}
				childsS=childsS->next;
			}
			childsAdvp=childsAdvp->next;
		}
		if (secondCondition) {
			childsAdvp=node.m_umbinarizedSubNodes;
			while(childsAdvp!=0){
				const CStateNode* sbarTarg=childsAdvp->node;
				if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
					CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
					while(childsSbar!=0){
						const CStateNode* inChildSbar=childsSbar->node;
						if ((*words)[inChildSbar->lexical_head].tag.code()==PENN_TAG_IN) {
							CStateNodeList* childsIn=inChildSbar->m_umbinarizedSubNodes;
							while(childsIn!=0){
								if (((*words)[childsIn->node->lexical_head].word==g_word_as)&&((*words)[childsIn->node->lexical_head].word==g_word_that)) {
									CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
									if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
										addLinked(&node,sbarTarg);
									    return true;
									}
								}
								childsIn=childsIn->next;
							}
						}
						childsSbar=childsSbar->next;
					}
				}


						//(*words)[childsSbar->node->lexical_head].tag.code()==PENN_TAG_IN
				childsAdvp=childsAdvp->next;
			}

		}

	}
	return false;
}

//"ADJP < (SBAR=target !< (IN < as) < S)"
    bool buildCComp9() {
    	if (node.constituent==PENN_CON_ADJP){
    		CStateNodeList* childsAdjp=node.m_umbinarizedSubNodes;
    		while(childsAdjp!=0){
    			const CStateNode* sbarTarg=childsAdjp->node;
    			if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
    				bool firstCondition=true;
    				CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
    				while(childsSbar!=0){
    					const CStateNode* inChildSbar=childsSbar->node;
    					if ((*words)[inChildSbar->lexical_head].tag.code()==PENN_TAG_IN) {
    						CStateNodeList* childsIn=inChildSbar->m_umbinarizedSubNodes;
    						while(childsIn!=0){
    							if ((*words)[inChildSbar->lexical_head].word==g_word_as) {

    							}
    							childsIn=childsIn->next;
    						}
    					}
    					childsSbar=childsSbar->next;
    				}
    				if (firstCondition){
    					childsSbar=sbarTarg->m_umbinarizedSubNodes;
    					while(childsSbar!=0){
    						const CStateNode* schildSbar=childsSbar->node;
    						if (schildSbar->constituent==PENN_CON_S){
    							CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
    							 if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
    								 addLinked(&node,sbarTarg);
    							 	 return true;
    							}
    						}
    						childsSbar=childsSbar->next;
    					}
    				}
    			}
    			childsAdjp=childsAdjp->next;
    		}
    	}
    	return false;
    }

    //"S <, (SBAR=target <, (IN < /^(?i:that|whether)$/) !$+ VP)",
    bool buildCComp10() {
    	if (node.constituent==PENN_CON_S){
    		CStateNodeList* childsS=node.m_umbinarizedSubNodes;
    		if (childsS!=0){ //<, first child only
    			const CStateNode* sbarTarg=childsS->node;
    			if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
    				CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
    				if (childsSbar!=0){ //<, first child only
    					const CStateNode* inChildSbar=childsSbar->node;
    					if ((*words)[inChildSbar->lexical_head].tag.code()==PENN_TAG_IN) {
    						// A $+ B. A is the immediate left sister of B
    						bool secondCondition=true;
    						if (childsSbar->next!=0){
    							const CStateNode* leftSisterIn = childsSbar->next->node;
    							if (leftSisterIn->constituent==PENN_CON_VP){
    								secondCondition=false;
    							}
    						}
    						if (secondCondition){
    							CStateNodeList* childsIn=inChildSbar->m_umbinarizedSubNodes;
    							while(childsIn!=0){
    								if (((*words)[childsIn->node->lexical_head].word==g_word_whether)&&((*words)[childsIn->node->lexical_head].word==g_word_that)) {
    									CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
    									if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
    										addLinked(&node,sbarTarg);
    										return true;
    									}
    								}
    								childsIn=childsIn->next;
    							}
    						}
    					}
    				}
    			}
    		}

    	}
    	return false;

    }
    //"@NP < JJ|NN|NNS < (SBAR=target [ !<(S < (VP < TO )) | !$-- NP|NN|NNP|NNS ] )"

           	   //SBAR=target !< (S<(VP<TO))      (SBAR does not have a child such as (S<...
           	   //SBAR=target!$-- NP|NN|NNP|NNS   (SBAR is not a right sister of NP ...)
        //but this is an OR
        //so, not at the same time...?

        bool buildCComp11() {
        	if (node.constituent==PENN_CON_NP){
        		CStateNodeList* childsNp=node.m_umbinarizedSubNodes;
        		while(childsNp!=0){
        			const CStateNode* jjnnnsChildNp=childsNp->node;
        			if (((*words)[jjnnnsChildNp->lexical_head].tag.code()==PENN_TAG_ADJECTIVE)
        					||((*words)[jjnnnsChildNp->lexical_head].tag.code()==PENN_TAG_NOUN)
        					||((*words)[jjnnnsChildNp->lexical_head].tag.code()==PENN_TAG_NOUN_PLURAL)) {

        				CStateNodeList* childsJJ=jjnnnsChildNp->m_umbinarizedSubNodes;
        				while(childsJJ!=0){
        					const CStateNode* sbarTarg=childsJJ->node;
        					if (sbarTarg->constituent==PENN_CON_SBAR && (!isLinked(&node,sbarTarg))) {
        						bool childCondition=true;
        						bool sisterCondition=true; //I assume at first sight that they are true.

        						CStateNodeList* leftSistersSbar=childsJJ->previous;
        						while(leftSistersSbar!=0){
        							const CStateNode* sister=leftSistersSbar->node;
        							if ((sister->constituent==PENN_CON_NP)
        							     ||((*words)[sister->lexical_head].tag.code()==PENN_TAG_NOUN)
        							     ||((*words)[sister->lexical_head].tag.code()==PENN_TAG_NOUN_PROPER)
        							     ||((*words)[sister->lexical_head].tag.code()==PENN_TAG_NOUN_PLURAL)) {
        								childCondition=false;
        							}
        							leftSistersSbar=leftSistersSbar->previous;
        						}

        						//SBAR=target !< (S<(VP<TO))
        						CStateNodeList* childsSbar=sbarTarg->m_umbinarizedSubNodes;
        						while(childsSbar!=0){
        							const CStateNode* schildSbar=childsSbar->node;
        							if (schildSbar->constituent==PENN_CON_S){
        								CStateNodeList* childsS=schildSbar->m_umbinarizedSubNodes;
        								while (childsS!=0){
        									const CStateNode* vpChild=childsS->node;
        									if (vpChild->constituent==PENN_CON_VP){
        										CStateNodeList* childsVp=vpChild->m_umbinarizedSubNodes;
        										while(childsVp!=0){
        											if (((*words)[childsVp->node->lexical_head].tag.code()==PENN_TAG_TO)){
        												childCondition=false;
        											}
        											childsVp=childsVp->next;
        										}
        									}
        									childsS=childsS->next;
        								}
        							}
        							childsSbar=childsSbar->next;
        						}

        						if (childCondition || sisterCondition) { //[] represents an or.
        							CDependencyLabel* label=new CDependencyLabel(STANFORD_DEP_CCOMP);
        							if (buildStanfordLink(label, sbarTarg->lexical_head, node.lexical_head)) {
        								addLinked(&node,sbarTarg);
        							    return true;
        							}
        						}
        					}
        					childsJJ=childsJJ->next;
        				}
        			}
        			childsNp=childsNp->next;
        		}
        	}
        	return false;

        }
