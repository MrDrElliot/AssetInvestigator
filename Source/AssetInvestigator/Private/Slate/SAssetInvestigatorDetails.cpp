// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SAssetInvestigatorDetails.h"

#include "BlueprintEditorModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Widgets/Notifications/SNotificationList.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetInvestigatorDetails::Construct(const FArguments& InArgs)
{
    AssetData = InArgs._AssetData;
    
    PopulateDependencyList();
    PopulateReferenceList();

    // Define filter options
    FilterOptions.Add(MakeShared<FString>(TEXT("None")));
    FilterOptions.Add(MakeShared<FString>(TEXT("Blueprints")));
    FilterOptions.Add(MakeShared<FString>(TEXT("Native")));
    FilterOptions.Add(MakeShared<FString>(TEXT("Materials")));
    FilterOptions.Add(MakeShared<FString>(TEXT("Assets")));
    CurrentFilter = "None";

    ChildSlot
    [
        SNew(SVerticalBox)
    
        // Button to Open Asset - Adjusted to better manage size and text placement
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight() // Change from FillHeight to AutoHeight to control the button's size
        [
            SNew(SButton)
            .Text(FText::FromString(TEXT("Open Asset")))
            .HAlign(HAlign_Center) // Center text horizontally
            .VAlign(VAlign_Center) // Center text vertically
            .OnClicked(this, &SAssetInvestigatorDetails::OnOpenAssetClicked)
        ]
    
        // Combo Box for selecting filter type
           + SVerticalBox::Slot()
           .AutoHeight()
           .Padding(10)
           [
               SNew(SComboBox<TSharedPtr<FString>>)
               .OptionsSource(&FilterOptions)
               .OnSelectionChanged(this, &SAssetInvestigatorDetails::OnFilterChanged)
               .OnGenerateWidget(this, &SAssetInvestigatorDetails::GenerateComboBoxWidget)
               .ContentPadding(FMargin(4.0, 2.0))
               .Content()
               [
                   SNew(STextBlock)
                   .Text(this, &SAssetInvestigatorDetails::GetCurrentFilterOption)
               ]
           ]
        
        // Section header for Asset Name - Enhanced visibility
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10, 5, 10, 0)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("Asset Name")))
            .ColorAndOpacity(FLinearColor::White) // Change text color to white for better readability
        ]
    
        // Display Asset Name
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text_Lambda([this] { return FText::FromName(AssetData.AssetName); })
            .ColorAndOpacity(FLinearColor::Gray)
        ]
        
        // Section header for Asset Path - Enhanced visibility
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(10, 5, 10, 0)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("Asset Path")))
            .ColorAndOpacity(FLinearColor::White) // Change text color to white for better readability
        ]
    
        // Display Asset Path
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text_Lambda([this]
            {
                return FText::FromString(AssetData.GetSoftObjectPath().ToString());
            })
            .ColorAndOpacity(FLinearColor::Gray)
        ]
    
        // Asset Dependencies List with header, border, and subtle outline
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor::White)
            .BorderImage(FCoreStyle::Get().GetBrush("Border"))
            .Padding(5)
            [
                CreateAssetListWidget(TEXT("Dependencies"), DependencyList)
            ]
        ]
    
        // Asset References List with header, border, and subtle outline
        + SVerticalBox::Slot()
        .Padding(10)
        .AutoHeight()
        [
            SNew(SBorder)
            .BorderBackgroundColor(FLinearColor::White)
            .BorderImage(FCoreStyle::Get().GetBrush("Border"))
            .Padding(5)
            [
                CreateAssetListWidget(TEXT("References"), ReferenceList)
            ]
        ]
    ];
}

void SAssetInvestigatorDetails::SetAssetData(const FAssetData& InData)
{
    AssetData = InData;
    PopulateDependencyList();
    PopulateReferenceList();
    
    Invalidate(EInvalidateWidgetReason::LayoutAndVolatility);
}

TSharedRef<SWidget> SAssetInvestigatorDetails::CreateAssetListWidget(FString ListTitle, const TSharedPtr<SVerticalBox>& ListContent)
{
    return SNew(SVerticalBox)
     + SVerticalBox::Slot()
     .AutoHeight() // This ensures the slot grows with the content, but within the confines of the SBox.
     [
         SNew(SBorder)
         .Padding(5)
         .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
         .ColorAndOpacity(FLinearColor::White)
         [
             SNew(SVerticalBox)
             + SVerticalBox::Slot()
             .AutoHeight()
             [
                 SNew(STextBlock)
                 .Text(FText::FromString(ListTitle))
                 .ColorAndOpacity(FLinearColor::White)
             ]
             + SVerticalBox::Slot()
             .Padding(5, 0, 5, 5)
             [
                 SNew(SBox)
                 .HeightOverride(200) // Correctly setting a fixed height for SScrollBox using SBox
                 [
                     SNew(SScrollBox)
                     + SScrollBox::Slot()
                     [
                         ListContent.ToSharedRef()
                     ]
                 ]
             ]
         ]
     ];
}

TSharedRef<SVerticalBox> SAssetInvestigatorDetails::PopulateDependencyList()
{

    if(!DependencyList.IsValid())
    {
        SAssignNew(DependencyList, SVerticalBox);
    }
    
    DependencyList->ClearChildren();
    Dependencies.Empty();
    
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    UE::AssetRegistry::FDependencyQuery DependencyQuery;
    DependencyQuery.Required = UE::AssetRegistry::EDependencyProperty::Hard;

    AssetRegistry.GetDependencies(FAssetIdentifier(AssetData.GetSoftObjectPath().GetLongPackageFName()), Dependencies, UE::AssetRegistry::EDependencyCategory::All, DependencyQuery);

    for (FAssetIdentifier& Dep : Dependencies)
    {
        // Getting the asset's package name
        FName PackageName = Dep.PackageName;
        if (!PackageName.IsNone() || !CurrentFilter.IsEmpty())
        {
            if(CurrentFilter != "None")
            {
                if(CurrentFilter == "Blueprint")
                {
                    if (Dep.PackageName.ToString().StartsWith(TEXT("/Script/")))
                    {
                        continue;
                    }
                }
            }
        }

        DependencyList->AddSlot()
        .AutoHeight()
        .Padding(2, 5)
        [
            SNew(SButton)
            .ContentPadding(FMargin(10, 2)) // Optional: Adjust padding inside the button
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0.0f, 0.0f, 8.0f, 0.0f)
                [
                    SNew(SImage)
                    .Image(FCoreStyle::Get().GetBrush("Graph.ExecPin.Connected"))
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                .VAlign(VAlign_Center) // Vertically center the text
                [
                    SNew(STextBlock)
                    .Text(FText::FromName(Dep.PackageName))
                ]
            ]
            .OnClicked_Lambda([this, Dep] { return OpenAssetEditor(Dep); })
        ];
    }
    return DependencyList.ToSharedRef();
}

TSharedRef<SVerticalBox> SAssetInvestigatorDetails::PopulateReferenceList()
{

    if(!ReferenceList.IsValid())
    {
        SAssignNew(ReferenceList, SVerticalBox);
    }

    ReferenceList->ClearChildren();
    References.Empty();
    
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    UE::AssetRegistry::FDependencyQuery DependencyQuery;
    DependencyQuery.Required = UE::AssetRegistry::EDependencyProperty::Hard;

    AssetRegistry.GetReferencers(FAssetIdentifier(AssetData.GetSoftObjectPath().GetLongPackageFName()), References, UE::AssetRegistry::EDependencyCategory::All, DependencyQuery);

    
    for (FAssetIdentifier& Ref : References)
    {
        

        ReferenceList->AddSlot()
        .AutoHeight()
        .Padding(2, 5)
        [
            SNew(SButton)
            .ContentPadding(FMargin(10, 2)) // Optional: Adjust padding inside the button
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0.0f, 0.0f, 8.0f, 0.0f)
                [
                    SNew(SImage)
                    .Image(FCoreStyle::Get().GetBrush("Graph.ExecPin.Connected"))
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                .VAlign(VAlign_Center) // Vertically center the text
                [
                    SNew(STextBlock)
                    .Text(FText::FromName(Ref.PackageName))
                ]
            ]
        ];
    }
    return ReferenceList.ToSharedRef();
}

FReply SAssetInvestigatorDetails::OpenAssetEditor(const FAssetIdentifier& Identifier)
{
    FVector2D WindowSize(800, 600);
    FString WindowTitle = TEXT("Asset Editor - Detailed References");

    TSharedPtr<SWindow> AssetEditorWindow = SNew(SWindow)
        .Title(FText::FromString(WindowTitle))
        .ClientSize(WindowSize)
        .SupportsMaximize(true)
        .SupportsMinimize(true);

    TSharedPtr<SVerticalBox> PropertyReferencesPanel = SNew(SVerticalBox);
    TSharedPtr<SVerticalBox> NodeReferencesPanel = SNew(SVerticalBox);

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FString AssetPath = AssetData.PackageName.ToString();
    UObject* LoadedAsset = AssetData.GetAsset();
    UBlueprint* Blueprint = Cast<UBlueprint>(LoadedAsset);

    const FAssetIdentifier& Referencer = Identifier;

    if (Blueprint && Blueprint->GeneratedClass && Blueprint->GeneratedClass->ClassGeneratedBy == Blueprint)
    {
        for (UEdGraph* Graph : Blueprint->UbergraphPages)
        {
            for (UEdGraphNode* Node : Graph->Nodes)
            {
                for (UEdGraphPin* Pin : Node->Pins)
                {
                    if (Pin->PinType.PinSubCategoryObject.IsValid() && Pin->PinType.PinSubCategoryObject->GetPackage()->GetName() == Referencer.PackageName)
                    {
                        NodeReferencesPanel->AddSlot()
                        .AutoHeight()
                        .Padding(10)
                        [
                            SNew(SButton)
                            .Text(FText::Format(FText::FromString("Node Reference: '{0}' in Blueprint: '{1}'"), FText::FromString(Node->GetName()), FText::FromString(Blueprint->GetName())))
                            .OnClicked(this, &SAssetInvestigatorDetails::OnNodeReferenceClicked, Node)
                        ];
                    }
                }
            }
        }

        FString RefAssetPath = Referencer.ToString();
        UObject* GeneratedObject = Blueprint->GeneratedClass->GetDefaultObject();
        for (const TPair<FObjectPropertyBase*, const void*>& PropertyValuePair : TPropertyValueRange<FObjectPropertyBase>(GeneratedObject->GetClass(), GeneratedObject, EPropertyValueIteratorFlags::FullRecursion))
        {
            const UClass* TestObj = PropertyValuePair.Key->PropertyClass;
            FString TestObjPath = TestObj->GetPackage()->GetPathName();
            if(TestObjPath == RefAssetPath)
            {
                FEdGraphPinType PinType = {};
                PinType.PinCategory = "object";
                const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
                FLinearColor Color = Schema->GetPinTypeColor(PinType);
                PropertyReferencesPanel->AddSlot()
                .Padding(5.0f)
                [
                    SNew(SButton)
                    .Text(FText::Format(FText::FromString("Property Reference: '{0}' in Blueprint: '{1}'"), FText::FromName(PropertyValuePair.Key->GetFName()), FText::FromString(Blueprint->GetName())))
                    .OnClicked(this, &SAssetInvestigatorDetails::OnPropertyReferenceClicked, PropertyValuePair.Key, Blueprint)
                ];
            }
            if(const FClassProperty* ClassProp = CastField<FClassProperty>(PropertyValuePair.Key))
            {
                const UClass* Metaclass = ClassProp->MetaClass;
                FString Path = Metaclass->GetPackage()->GetPathName();
                if(Path == RefAssetPath)
                {
                    FEdGraphPinType PinType = {};
                    PinType.PinCategory = "class";
                    const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
                    FLinearColor Color = Schema->GetPinTypeColor(PinType);
                    TSharedPtr<IPropertyHandle> InPropertyHandle;
                    PropertyReferencesPanel->AddSlot()
                    .Padding(5.0f)
                    [
                        SNew(SButton)
                        .Text(FText::Format(FText::FromString("Property Reference: '{0}' in Blueprint: '{1}'"), FText::FromName(PropertyValuePair.Key->GetFName()), FText::FromString(Blueprint->GetName())))
                        .OnClicked(this, &SAssetInvestigatorDetails::OnPropertyReferenceClicked, PropertyValuePair.Key, Blueprint)
                    ];
                }
            }
        }

        for (const TPair<FArrayProperty*, const void*>& PropertyValuePair : TPropertyValueRange<FArrayProperty>(GeneratedObject->GetClass(), GeneratedObject, EPropertyValueIteratorFlags::FullRecursion))
        {
            FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(PropertyValuePair.Key->Inner);
            if(ObjectProperty == nullptr) continue;
            
            const UClass* TestObj = ObjectProperty->PropertyClass;
            FString TestObjPath = TestObj->GetPackage()->GetPathName();
            if(TestObjPath == RefAssetPath)
            {
                FEdGraphPinType PinType = {};
                PinType.PinCategory = "object";
                const UEdGraphSchema_K2* Schema = GetDefault<UEdGraphSchema_K2>();
                FLinearColor Color = Schema->GetPinTypeColor(PinType);
                PropertyReferencesPanel->AddSlot()
                .Padding(5.0f)
                [
                    SNew(SButton)
                    .Text(FText::Format(FText::FromString("Property Reference: '{0}' in Blueprint: '{1}'"), FText::FromName(PropertyValuePair.Key->GetFName()), FText::FromString(Blueprint->GetName())))
                    .OnClicked(this, &SAssetInvestigatorDetails::OnPropertyReferenceClicked, ObjectProperty, Blueprint)
                ];
            }
        }
    }
    
    TSharedPtr<SScrollBox> ScrollBox = SNew(SScrollBox)
         + SScrollBox::Slot()
         [
             SNew(SBorder)
             .Padding(10)
             .BorderBackgroundColor(FLinearColor(0.1, 0.1, 0.1, 0.5))
             .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.DarkGroupBorder"))
             [
                 SNew(STextBlock)
                 .Text(FText::FromString("Property References"))
                 .Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
                 .ColorAndOpacity(FLinearColor::White)
             ]
         ]
         + SScrollBox::Slot()
         [
             SNew(SBorder)
             .Padding(15)
             .BorderBackgroundColor(FLinearColor(0.2, 0.2, 0.2, 1.0))
             .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.LightGroupBorder"))
             [
                 PropertyReferencesPanel.ToSharedRef()
             ]
         ]
         + SScrollBox::Slot()
         [
             SNew(SBorder)
             .Padding(10)
             .BorderBackgroundColor(FLinearColor(0.1, 0.1, 0.1, 0.5))
             .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.DarkGroupBorder"))
             [
                 SNew(STextBlock)
                 .Text(FText::FromString("Node References"))
                 .Font(FCoreStyle::Get().GetFontStyle("EmbossedText"))
                 .ColorAndOpacity(FLinearColor::White)
             ]
         ]
         + SScrollBox::Slot()
         [
             SNew(SBorder)
             .Padding(15)
             .BorderBackgroundColor(FLinearColor(0.2, 0.2, 0.2, 1.0))
             .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.LightGroupBorder"))
             [
                 NodeReferencesPanel.ToSharedRef()
             ]
         ];

    AssetEditorWindow->SetContent(SNew(SBorder)
        .Padding(15)
        .BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        [
            ScrollBox.ToSharedRef()
        ]);

    FSlateApplication::Get().AddWindow(AssetEditorWindow.ToSharedRef());

    return FReply::Handled();
}

FReply SAssetInvestigatorDetails::OnOpenAssetClicked()
{
    if (AssetData.IsValid())
    {
        // Display a notification that loading is starting
        FText NotificationText = FText::Format(NSLOCTEXT("AssetInvestigator", "LoadingAsset", "Loading asset: {0}..."), FText::FromName(AssetData.AssetName));
        FNotificationInfo Info(NotificationText);
        Info.bFireAndForget = false;
        Info.FadeOutDuration = 0.0f;
        Info.ExpireDuration = 0.0f;

        auto NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
        NotificationItem->SetCompletionState(SNotificationItem::CS_Pending);

        // Start loading the asset
        UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(AssetData.GetSoftObjectPath(), FStreamableDelegate::CreateLambda([this, NotificationItem]()
        {
            // Assuming you have access to GEditor and it's valid
            if (GEditor && AssetData.GetAsset())
            {
                GEditor->EditObject(AssetData.GetAsset());
            }

            // Update the notification to reflect that the asset has been loaded
            NotificationItem->SetText(NSLOCTEXT("AssetInvestigator", "AssetLoaded", "Asset loaded successfully."));
            NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
            NotificationItem->ExpireAndFadeout();
        }));
    }
    return FReply::Handled();
}

FReply SAssetInvestigatorDetails::OnNodeReferenceClicked(UEdGraphNode* Node)
{
    if (!Node)
    {
        return FReply::Unhandled();
    }

    // Ensure the node's blueprint is loaded and valid
    UObject* GraphObject = Node->GetOuter();
    while (GraphObject && !GraphObject->IsA(UBlueprint::StaticClass()))
    {
        GraphObject = GraphObject->GetOuter();
    }
    
    UBlueprint* Blueprint = Cast<UBlueprint>(GraphObject);
    if (!Blueprint)
    {
        return FReply::Unhandled();
    }

    // Open the blueprint in the editor
    if (GEditor)
    {
        GEditor->EditObject(Blueprint);
    }

    // Fetch the editor for the blueprint
    if (FModuleManager::Get().IsModuleLoaded("Kismet"))
    {
        IAssetEditorInstance* Editor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Blueprint, true);
        if (Editor)
        {
            // Cast to a Blueprint editor if possible
            IBlueprintEditor* BlueprintEditor = static_cast<IBlueprintEditor*>(Editor);
            if (BlueprintEditor)
            {
                // Focus on the specific node
                FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(Node, false);

            }
        }
    }

    return FReply::Handled();
}

FReply SAssetInvestigatorDetails::OnPropertyReferenceClicked(FObjectPropertyBase* Property, UBlueprint* Blueprint)
{
    if (Blueprint && GEditor)
    {
        GEditor->EditObject(Blueprint);

        // Get the currently open editor for the Blueprint
        IAssetEditorInstance* AssetEditorInstance = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Blueprint, false);
        {
            // This is the interface to the Blueprint editor, but casting it might be required and tricky
            IBlueprintEditor* BlueprintEditor = static_cast<IBlueprintEditor*>(AssetEditorInstance);
            if (BlueprintEditor)
            {
            }
        }
    }

    return FReply::Handled();
}

TSharedRef<SWidget> SAssetInvestigatorDetails::GenerateComboBoxWidget(TSharedPtr<FString> InOption)
{
    return SNew(STextBlock)
        .Text(FText::FromString(*InOption))
        .Font(FCoreStyle::Get().GetFontStyle("NormalFont"));
}

void SAssetInvestigatorDetails::OnFilterChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo)
{
    CurrentFilter = *NewValue;
    PopulateDependencyList();
    PopulateReferenceList();
}

FText SAssetInvestigatorDetails::GetCurrentFilterOption() const
{
    return FText::FromString(CurrentFilter.IsEmpty() ? TEXT("Select Filter...") : CurrentFilter);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
