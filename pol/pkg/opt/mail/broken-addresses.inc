///////////////////////////////////////////////////////////////////////////
// enthaelt aus Bounces extrahiert kaputte Adressen (lowercase)

Function IsBrokenAddress(addr)
    If (addr in (array{
    }))
        return 1;
    EndIf

    return 0;
EndFunction

