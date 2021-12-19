#pragma once

#include "cHook.h"
#include "essential.h"
#include "network.h"
#include "protocol.h"

class cConHook : public cHook
{
public:
   cConHook(void);
   ~cConHook(void);

   void        Close(int bNotifyMud);
   char        AddInputChar(uint8_t c);
   void        AddString(char *str);
   void        ParseInput(void);
   void        SendCon(const char *str);
   void        WriteCon(const char *str);
   char       *IndentText(const char *source, char *dest, int dest_size, int width);
   const char *ParseOutput(const char *text);
   void        PromptErase(void);
   void        PromptRedraw(const char *prompt);
   void        TransmitCommand(const char *text);
   void        ShowChunk(void);
   void        ProcessPaged(void);
   void        PressReturn(const char *cmd);
   void        PlayLoop(const char *cmd);
   void        MudDown(const char *cmd);
   void        MenuSelect(const char *cmd);
   void        SequenceCompare(uint8_t *pBuf, int *pnLen);

   void        Input(int nFlags);
   void        getLine(uint8_t buf[], int *size);
   void        testChar(uint8_t c);

   uint16_t    m_nId;
   int         m_nFirst;
   uint8_t     m_nLine;
   int         m_nPromptMode; /* 0 none, 1 press return */
   int         m_nPromptLen;  /* Prompt length          */
   int         m_nSequenceCompare;

   int         m_nState;
   int         m_nEscapeCode; /* Very simplified state diagram assistance */
   char        m_aOutput[4096];
   char        m_aInputBuf[2 * MAX_INPUT_LENGTH];
   char        m_aHost[50];
   void (*m_pFptr)(class cConHook *, const char *cmd);

   cConHook                  *m_pNext;

   struct terminal_setup_type m_sSetup;

   uint8_t                    m_nBgColor; /* Stupid bitching ANSI   */

   cQueue                     m_qInput; /* Input from user        */
   cQueue                     m_qPaged; /* Paged text output      */
};
