import router from '@system.router'
import resourceManager from '@ohos.resourceManager';

export default {
    data: {
        shareHapList: [],
        lineNums: 6,
        iconBorad: "0.25vp solid #000000",
        viewStyle: {
            textVisible: "visible",
            copyVisible: "hidden",
            printVisible: "hidden",
            textMainHeight: "19vp",
            icon: "",
            iconBoard: "",
            format: "png",
            button1: "",
            button2: "",
        },
        share: {
            width: "519vp",
            height: "256vp",
            backTop: "6vp",
            backIcon: "common/handle_down.png",
            card: {
                width: "495vp",
                height: "87vp",
                radius: "24vp",
                contentMargin: "16vp 43.5vp 6vp 16vp",
                text: {
                    top: "3vp",
                    left: "16vp",
                    main:{
                        width: "380vp",
                    },
                    sub: {
                        top: "2vp",
                        width: "380vp",
                    },
                },
                btn:{
                    left: "405vp",
                }
            },
            swiper: {
                height: "115vp",
                select: {
                    marginTop: "16vp",
                }
            }
        },
    },
    onInit() {
        if (this.deviceType == "phone") {
            console.log("this is phone devices");
            this.getHapListStyle();
        }

        this.getViewStyle();
        let shareHap = [];
        for (let i = 0; i < this.hapList.length; i++) {
            shareHap.push(this.hapList[i]);
            if (i % this.lineNums == this.lineNums - 1 || i == this.hapList.length - 1) {
                this.shareHapList.push(shareHap);
                shareHap = [];
            }
        }
        this.getHapResource();
    },
    onShare: function (item) {
        let param = item.bundle + ";" + item.ability;
        console.log("dialog service share to :" + item.ability);
        callNativeHandler('SHARE_EVENT', param);
    },
    changeSize: function (param) {
        callNativeHandler('CLOSE_EVENT', "");
    },
    onBack: function () {
        console.log("dialog service close");
        callNativeHandler('CLOSE_EVENT', "");
    },
    getViewStyle() {
        this.viewStyle.button1 = this.$t('strings.btnCopy');
        this.viewStyle.button2 = this.$t('strings.btnPrint');
        let defaultIcon = "";
        let fileNums = this.previewCard.fileList == null ? 0 : this.previewCard.fileList.length;
        if (this.previewCard.type == "text/plain") {
            this.viewStyle.textVisible = "hidden"
            this.viewStyle.textMainHeight = "38vp";
            defaultIcon = "ic_documents";
        } else if (this.previewCard.type == "text/html") {
            defaultIcon = "ic_html";
            this.previewCard.subText = this.$t('strings.uri') + this.previewCard.subText;
        } else if (fileNums == 0) {
            defaultIcon = "ic_unknown";
            if (this.previewCard.type == null) {
                if (this.previewCard.subText != null) {
                    defaultIcon = "ic_html";
                    this.previewCard.subText = this.$t('strings.uri') + this.previewCard.subText;
                } else if (this.previewCard.mainText != null) {
                    this.viewStyle.textVisible = "hidden"
                    this.viewStyle.textMainHeight = "38vp";
                    defaultIcon = "ic_documents";
                }
            }
            if (defaultIcon == "ic_unknown") {
                this.previewCard.mainText = this.$t('strings.defaultName');
                this.previewCard.subText = this.$t('strings.fileSize') + " KB";
            }
        } else if (fileNums == 1) {
            this.previewCard.mainText = this.previewCard.fileList[0].name;
            this.previewCard.subText = this.$t('strings.fileSize') + Math.round(this.previewCard.fileList[0].size / 1024) + "KB";
            if (this.previewCard.type.indexOf("image/") != -1) {
                defaultIcon = "ic_image";
            } else if (this.previewCard.type.indexOf("application/apk") != -1) {
                defaultIcon = "ic_apk";
            } else if (this.previewCard.type.indexOf("application/pdf") != -1) {
                defaultIcon = "ic_pdf";
            } else if (this.previewCard.type.indexOf("application/doc") != -1) {
                defaultIcon = "ic_doc";
            } else if (this.previewCard.type.indexOf("application/ppt") != -1) {
                defaultIcon = "ic_pptx";
            } else if (this.previewCard.type.indexOf("application/xls") != -1) {
                defaultIcon = "ic_xls";
            } else if (this.previewCard.type.indexOf("application/ics") != -1) {
                defaultIcon = "ic_calendar";
            } else if (this.previewCard.type.indexOf("application/vcf") != -1) {
                defaultIcon = "ic_contacts";
            } else if (this.previewCard.type.indexOf("video/") != -1) {
                defaultIcon = "ic_video";
            } else if (this.previewCard.type.indexOf("audio/") != -1) {
                defaultIcon = "ic_mp3";
            } else {
                defaultIcon = "ic_unknown";
            }
            if (this.previewCard.mainText == "") {
                this.previewCard.mainText = this.$t('strings.defaultName');
            }
        } else {
            defaultIcon = "ic_file_multiple";
            if (this.previewCard.type.indexOf("*") != -1) {
                this.previewCard.mainText = fileNums + this.$t('strings.sameFile');
            } else {
                this.previewCard.mainText = fileNums + this.$t('strings.unSameFile');
            }
            let totalSize = 0;
            for (let i = 0; i < fileNums; i++) {
                if (this.previewCard.fileList[i].size != null) {
                    totalSize += this.previewCard.fileList[i].size;
                }
            }
            this.previewCard.subText = this.$t('strings.totalSize') + Math.round(totalSize / 1024) + "KB";
        }
        this.viewStyle.icon = "common/" + defaultIcon + "." + this.viewStyle.format;
    },
    getHapListStyle() {
        if (this.deviceType == "phone") {
            this.lineNums = 8;
        }
        this.share.width = "360vp";
        this.share.height = "347vp";
        this.share.backTop = "0vp";
        this.share.card.width = "336vp";
        this.share.card.height = "96vp";
        this.share.card.radius = "16vp";
        this.share.card.contentMargin = "16vp 24vp 8vp 16vp";
        this.share.card.text.top = "6vp";
        this.share.card.text.left = "12vp";
        this.share.card.text.main.width = "236vp";
        this.share.card.text.sub.top = "4vp";
        this.share.card.text.sub.width = "236vp";
        this.share.card.btn.left = "254vp";
        this.share.swiper.height = "200vp";
        this.share.swiper.select.marginTop = "8vp";
    },
    getHapResource() {
        this.labelMap = new Map()
        for (let i = 0; i < this.hapList.length; i++) {
            let lableId = Number(this.hapList[i].name);
            resourceManager.getResourceManager(this.hapList[i].bundle).then(mgr =>{
                console.log("dialog service bundle:" + this.hapList[i].bundle + "---lableId:" + lableId);
                mgr.getString(lableId).then(value => {
                    console.log("dialog service get label(" + lableId + ") value:" + value);
                    this.updateHapName(this.hapList[i].bundle, value);
                }).catch(error => {
                    console.log("dialog service resource getString error:" + error);
                })
            }).catch(error => {
                console.log("dialog service getResourceManager error:" + error);
            });

            let iconId = Number(this.hapList[i].icon);
            resourceManager.getResourceManager(this.hapList[i].bundle).then(mgr =>{
                console.log("dialog service bundle:" + this.hapList[i].bundle + "---iconId:" + iconId);
                mgr.getMediaBase64(iconId).then(value => {
                    console.log("dialog service get icon(" + iconId + ") value:" + value);
                    this.updateHapIcon(this.hapList[i].bundle, value);
                }).catch(error => {
                    console.log("dialog service resource getString error:" + error);
                })
            }).catch(error => {
                console.log("dialog service getResourceManager error:" + error);
            });
        }
    },
    updateHapName(bundle, hapLabel) {
        for (let i = 0; i < this.shareHapList.length; i++) {
            if (this.shareHapList[i] != null) {
                for (let j = 0; j < this.shareHapList[i].length; j++) {
                    if (this.shareHapList[i][j].bundle == bundle) {
                        this.shareHapList[i][j].name = hapLabel;
                        console.log("dialog service update bundle:" + bundle + " to lable:" + hapLabel);
                    }
                }
            }
        }
    },
    updateHapIcon(bundle, hapIcon) {
        for (let i = 0; i < this.shareHapList.length; i++) {
            if (this.shareHapList[i] != null) {
                for (let j = 0; j < this.shareHapList[i].length; j++) {
                    if (this.shareHapList[i][j].bundle == bundle) {
                        this.shareHapList[i][j].icon = hapIcon;
                        console.log("dialog service update bundle:" + bundle + " to icon:" + hapIcon);
                    }
                }
            }
        }
    }
}
