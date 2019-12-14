// Buecherregale beim Startup neubestuecken?
Const GPROP_RECREATE_BOOKS := "resetbookcases";
Const PROP_CASE_AUTOSPAWN  := "globalized";
Const PROP_BOOK_ID         := "bookid";
Const PROP_BOOK_NO_REMOVE  := "noremove";

Function IsBook(item)
    If (item.objtype in (array{
        // "Zauberbuch"
        0x0e3b,
        // schraeges Buch fuer Buchstaender
        0x0ebd, 0x0ebe, 0x0ebf, 0x0ec0,
        // aufgeschlagenes Buch
        0x0fbd, 0x0fbe,
        // verschiedene bunte Buecher
        0x0fef, 0x0ff0, 0x0ff1, 0x0ff2, 0x0ff3, 0x0ff4,
        0x1e20, 0x1c13, 0x238c,
        // Buecherstapel etc.
        0x1e21, 0x1e22, 0x1e23, 0x1e24, 0x1e25,
        // spezielle Buecher (Zaubern, Alchemie, Musik etc.)
        0x2d50, 0x6100, 0x6101, 0x6695, 0x6820, 0x6821, 0x6822,
        0xe686, 0xff85
    }))
        return 1;
    EndIf

    return 0;
EndFunction
